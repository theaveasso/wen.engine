#include "game.h"
#include "component.h"

void make_entities_from_scene(flecs::world &world, gfx::Scene &scene);

HWGame::HWGame()
{
	_app_config.appName = "Hello world!";
}

void HWGame::on_init()
{
	_world = flecs::world();

	_mesh_cache     = make_shared<wvk::gfx::MeshCache>(_instance);
	_material_cache = make_shared<gfx::MaterialCache>(_instance);

	auto drawTextureId = _instance.make_texture({.format = _format, .extent = {WVK_CAST(uint32_t, _app_config.renderArea.x), WVK_CAST(uint32_t, _app_config.renderArea.y), 1}});
	_draw_texture      = _instance.get_texture(drawTextureId);

	// load levels
	WVK_PROFILER_ZONE("loadScene", WVK_PROFILER_COLOR_CREATE);
	_current_scene = gfx::scene_load(
	    _instance,
	    _mesh_cache,
	    _material_cache,
	    std::filesystem::current_path() / "etc/model/sea_house/scene.gltf");

	// spawn entity
	make_entities_from_scene(_world, _current_scene);
	WVK_PROFILER_ZONE_END()

	_renderer = std::make_unique<wvk::gfx::DeferredRenderer>(
	    _instance,
	    _mesh_cache,
	    _material_cache,
	    glm::vec2(_app_config.renderArea));

	{
		// load ldtk tilemap
		wvk::core::load_ldtk_json(
		    std::filesystem::current_path() / "etc/data/hello_world.ldtk",
		    &_ldtk_json);

		// add texture
		for (auto tileset : _ldtk_json.get_defs().get_tilesets())
		{
			auto relPath = tileset.get_rel_path();
			if (!relPath.has_value())
			{
				continue;
			}

			wvk::gfx::ImageData textureData;
			wvk::gfx::load_image(std::filesystem::current_path() / "etc/data" / tileset.get_rel_path().value(), &textureData);

			wvk::TextureId textureId = _instance.make_texture_w_pixels(
			    VK_FORMAT_R8G8B8A8_SRGB,
			    VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
			    VkExtent2D{WVK_CAST(uint32_t, textureData.width), WVK_CAST(uint32_t, textureData.height)},
			    textureData.pixels,
			    tileset.get_rel_path().value());

			_instance.register_tileset_texture(relPath.value(), textureId);
			_instance.get_texture(textureId)->set_size2d(glm::vec2(textureData.width, textureData.height));
		}
	}

	_sprite_renderer.init(_instance, _format);

	_game_camera.init_ortho_2d(WVK_CAST(glm::vec2, get_game_screen_size()));
}

void HWGame::on_update(float dt)
{
	ImGui::SetCurrentContext(_instance.get_imgui_ctx());
	ImGui::ShowDemoWindow();
}

void HWGame::on_draw()
{
	VkCommandBuffer cmd = _instance.begin_frame();

	wvk::gfx::transition_image(
	    cmd,
	    _draw_texture->handle(),
	    VK_IMAGE_LAYOUT_UNDEFINED,
	    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	const glm::vec4 clearColor = glm::vec4{0.1f, 0.1f, 0.1f, 1.0f};

	wvk::gfx::clear(cmd, _draw_texture, clearColor);

	_renderer->begin_draw();

	WVK_PROFILER_ZONE("generateDrawList", WVK_PROFILER_COLOR_CREATE);
	auto meshQuery = _world.query<MeshComponent>();
	meshQuery.each([&](MeshComponent &mc) {
		for (size_t i = 0; i < mc.meshes.size(); ++i)
		{
			const auto meshTransform = glm::mat4(1.0f);
			_renderer->draw_mesh(mc.meshes[i], mc.meshMaterials[i], mc.castShadow, meshTransform);
		}
	});
	WVK_PROFILER_ZONE_END()

	_renderer->draw(cmd, _instance);
	_renderer->end_draw();

	_sprite_renderer.begin_draw();
	draw_world();
	_sprite_renderer.end_draw();
	_sprite_renderer.draw(cmd, _instance, _draw_texture, _game_camera.get_view_proj());

	const wvk::EndFrameParams endFrameParams{.drawImGui = true};

	_instance.end_frame(cmd, _draw_texture, endFrameParams);
}
void HWGame::on_cleanup()
{
	_instance.wait_idle();

	_sprite_renderer.cleanup(_instance);
}
void HWGame::load_app_settings()
{
	const std::filesystem::path appSettingsPath{"etc/data/default_app_settings.json"};
	wvk::core::JsonFile         file(appSettingsPath);
	if (!file.is_good())
	{
		WVK_ERROR("failed to load app settings from {}", appSettingsPath.string());
		return;
	}
}
void HWGame::draw_world()
{
	const auto &levels = _ldtk_json.get_levels();

	const auto &activeLevel = levels[0];

	_tilemap_renderer.draw_level(_instance, _sprite_renderer, activeLevel);
}
void HWGame::draw_game_objects()
{
}
void HWGame::draw_ui()
{
}
glm::ivec2 HWGame::get_game_screen_size() const
{
	return _app_config.renderArea;
}
glm::vec2 HWGame::get_mouse_game_screen_position() const
{
	return glm::vec2();
}
glm::vec2 HWGame::get_mouse_world_position() const
{
	return glm::vec2();
}

void make_entities_from_scene(flecs::world &world, gfx::Scene &scene)
{
	for (const auto &mesh : scene.meshes)
	{
		world.entity()
		    .set<MeshComponent>({.meshes         = mesh.primitives,
		                         .meshMaterials  = mesh.primitiveMaterials,
		                         .meshTransforms = std::vector<core::Transform>{}});
	}
}
