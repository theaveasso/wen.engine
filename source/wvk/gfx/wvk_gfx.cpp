#include "wvk_gfx.hpp"

#include <GLFW/glfw3.h>
#include <VkBootstrap.h>
#include <backends/imgui_impl_glfw.h>
#include <imgui.h>
#include <stb_image.h>

#include <glm/gtx/string_cast.hpp>

#include "wvk_init.gfx.hpp"
#include "wvk_inst.gfx.hpp"
#include "wvk_instimpl.gfx.hpp"
#include "wvk_types.gfx.hpp"
#include "wvk_vulkan.gfx.hpp"

#include "wvk/core/wvk_gltf.hpp"
#include "wvk/core/wvk_ldtk.hpp"

namespace wvk::gfx
{

// ---------------------------------------------------------------------------------------------
// gfx::CubeMap
// ---------------------------------------------------------------------------------------------
ImageData::~ImageData() {
    stbi_image_free(pixels);
}

shared_ptr<Texture> loadCubeMap(
    Instance                    &instance,
    const std::filesystem::path &imageDir)
{
	shared_ptr<Texture> outTexture;

	static const auto paths =
	    std::array{"right.jpg", "left.jpg", "top.jpg", "bottom.jpg", "front.jpg", "back.jpg"};

	//	uint32_t face         = 0;
	bool     imageCreated = false;

	const std::string label = "cubemap_" + imageDir.string();
	for (auto &p : paths)
	{
		ImageData data;
		load_image(imageDir / p, &data);
		WVK_ASSERT_MSG(data.channels == 4,
		               fmt::format("expected image to have 4 channels (RGBA) but got {}", data.channels));

		if (!imageCreated)
		{
			CreateTextureInfo textureInfo = {
			    .format    = VK_FORMAT_R8G8B8A8_SRGB,
			    .usages         = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			    .flags     = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
			    .extent    = {.width = WVK_CAST(uint32_t, data.width), .height = WVK_CAST(uint32_t, data.height), .depth = 1},
			    .baseArrayLayer = 6,
			    .sampler   = VK_SAMPLE_COUNT_1_BIT,
			    .tiling    = VK_IMAGE_TILING_OPTIMAL,
			    .mipMap    = false,
			    .isCubeMap = true,
			};

			VmaAllocationCreateInfo memInfo =
			    init::vma_allocation_create_info();

			imageCreated = true;
			outTexture   = make_shared<Texture>(
                instance.get_device(),
                instance.get_allocator(),
                textureInfo,
                memInfo,
                "cubemap_" + std::string(p) + "_" + imageDir.string());
		}
		else
		{
			WVK_ASSERT_MSG(outTexture->get_extent_2d().width == WVK_CAST(uint32_t, data.width) &&
			                   outTexture->get_extent_2d().height == WVK_CAST(uint32_t, data.height),
			               "all images for cubemap must have the same size!");
		}
	}

	return outTexture;
}

void load_image(const std::filesystem::path &path, ImageData *image)
{
	image->pixels   = stbi_load(path.string().c_str(), &image->width, &image->height, &image->channels, STBI_rgb_alpha);
	image->channels = 4;
	WVK_ASSERT_MSG(image->pixels, "failed to load image, return nullptr from stbi_load");
}

// ---------------------------------------------------------------------------------------------
// Material
// ---------------------------------------------------------------------------------------------

MaterialCache::MaterialCache(
    Instance &instance)
{
	_materialDataBuffer = instance.make_gpu_buffer(
	    MAX_MATERIALS * sizeof(MaterialData),
	    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);

	uint32_t normal = 0xFFFF8080;        // {0.5, 0.5, 1.0, 1.0}

	_default_normal_map_texture_id = instance.make_texture_w_pixels(
	    VK_FORMAT_R8G8B8A8_UNORM,
	    VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
	    VkExtent2D{1, 1},
	    &normal,
	    "default_normal_material");

	const Material placeholderMaterial = {
	    .name = "BUILTIN_MATERIAL"};

	_placeholder_material_id = add_material(instance, placeholderMaterial);
}

void MaterialCache::cleanup()
{
	_materialDataBuffer->cleanup();
}

MaterialId MaterialCache::add_material(Instance &instance, Material material)
{
	const auto getTextureOrElse =
	    [](TextureId tid, TextureId placeholder) {
		    return tid != NULL_TEXTURE_ID ? tid : placeholder;
	    };

	TextureId whiteTextureId = instance.get_white_texture_id();

	TextureId id = get_free_material_id();

	VkDeviceSize offset = id * sizeof(MaterialData);

	MaterialData data           = {};
	data.baseColor              = material.baseColor;
	data.diffuseTex             = getTextureOrElse(material.diffuseTexture, whiteTextureId);
	data.normalTex              = getTextureOrElse(material.normalMapTexture, _default_normal_map_texture_id);
	data.emissiveTex            = getTextureOrElse(material.emissiveTexture, whiteTextureId);
	data.metallicRoughnessTex   = getTextureOrElse(material.metallicRoughnessTexture, whiteTextureId);
	data.metalRoughnessEmissive = glm::vec4(material.metallicFactor, material.roughnessFactor, material.emissiveFactor, 0.0f);

	instance.upload_buffer_to_gpu(
	    _materialDataBuffer,
	    WVK_CAST(void *, &data),
	    sizeof(MaterialData), offset);

	_materials.push_back(std::move(material));
	return id;
}

MaterialId MaterialCache::get_placeholder_material_id() const
{
	WVK_ASSERT_MSG(_placeholder_material_id != NULL_MATERIAL_ID, "default material placeholder is not available");
	return _placeholder_material_id;
}

VkDeviceAddress MaterialCache::get_material_data_buffer_address(VkDevice device) const
{
	return _materialDataBuffer->get_device_address(device);
}

#pragma region gfx::Sprite
Sprite::Sprite(const std::shared_ptr<Texture> &texture)
{
	set_texture(texture);
}
void Sprite::set_texture(const std::shared_ptr<Texture> &t)
{
	texture_id   = t->get_bindless_id();
	texture_size = t->get_size_2d();
}

void Sprite::set_texture_size(glm::vec2 size)
{
	texture_size = size;
}

void Sprite::set_texture_rect(const core::Rectangle &)
{
}
void Sprite::set_pivot_pixel(const glm::ivec2 &pixel)
{
	WVK_ASSERT_MSG(texture_id != NULL_BINDLESS_ID, "");
	pivot = WVK_CAST(glm::vec2, pixel) / texture_size;
}
#pragma endregion

// ---------------------------------------------------------------------------------------------
// Sprite Renderer
// ---------------------------------------------------------------------------------------------
#pragma region gfx::SpriteRenderer

void SpriteRenderer::init(
    Instance        &instance,
    VkFormat         drawTextureFormat,
    std::string_view name)
{
	_sprite_draw_commands.reserve(MAX_SPRITES);

	_drawing_pipeline = SpriteRenderingPipeline(instance, drawTextureFormat, MAX_SPRITES, name);

	_is_initialized = true;
}

void SpriteRenderer::cleanup(Instance &instance)
{
	_drawing_pipeline.cleanup(instance);
}

void SpriteRenderer::begin_draw()
{
	WVK_ASSERT_MSG(_is_initialized, "sprite renderer was not initialized");
	_sprite_draw_commands.clear();
}

void SpriteRenderer::end_draw()
{
	// do nothing
}

void SpriteRenderer::draw(
    VkCommandBuffer           &cmd,
    Instance                  &instance,
    const shared_ptr<Texture> &drawImage)
{
	core::Camera uiCamera;
	draw(cmd, instance, drawImage, uiCamera.get_view_proj());
}

void SpriteRenderer::draw(
    VkCommandBuffer           &cmd,
    Instance                  &instance,
    const shared_ptr<Texture> &drawImage,
    const glm::mat4           &viewProjection)
{
	_drawing_pipeline.draw(
	    cmd,
	    instance,
	    drawImage,
	    viewProjection,
	    _sprite_draw_commands);
}

static Sprite make_sprite(
    TextureId        textureId,
    const glm::vec2 &textureSize,
    const ColorRGBA &tint)
{
	Sprite outSprite;
	outSprite.texture_id   = textureId;
	outSprite.texture_size = textureSize;
	outSprite.color        = tint;
	outSprite.uv0          = {0.0, 0.0};
	outSprite.uv1          = {1.0, 1.0};
	outSprite.pivot        = {0.0, 0.0};
	return outSprite;
}

void SpriteRenderer::draw_sprite(
    const Sprite    &sprite,
    const glm::vec2 &position,
    float            rotation,
    const glm::vec2 &scale,
    uint32_t         shaderId)
{
	core::Transform transform;
	transform.set_position(glm::vec3{position, 0.f});
	if (rotation != 0)
	{
		transform.set_heading(glm::angleAxis(rotation, glm::vec3{0.f, 0.f, 1.f}));
	}
	transform.set_scale(glm::vec3{scale, 1.f});

	draw_sprite(sprite, transform.as_matrix(), shaderId);
}

void SpriteRenderer::draw_sprite(
    const Sprite    &sprite,
    const glm::mat4 &transform,
    uint32_t         shaderId)
{
	WVK_ASSERT_MSG(sprite.texture_id != NULL_BINDLESS_ID, "sprite texture invalid!");

	_sprite_draw_commands.push_back(
	    SpriteDrawCommand{
	        .transform = transform,
	        .uv0       = sprite.uv0,
	        .uv1       = sprite.uv1,
	        .color     = sprite.color,
	        .textureId = sprite.texture_id,
	        .shaderId  = shaderId,
	    });
}

void SpriteRenderer::draw_text()
{
}

void SpriteRenderer::draw_rectangle(
    Instance        &instance,
    int              posX,
    int              posY,
    int              width,
    int              height,
    const glm::vec4 &color)
{
	core::Rectangle rec{};
	rec.set_position(glm::vec2{posX, posY});
	rec.set_size(glm::vec2{width, height});
	draw_rectangle_pro(instance, rec, glm::vec2{0.0f}, 0.0f, color);
}

void SpriteRenderer::draw_rectangle_v(
    Instance        &instance,
    const glm::vec2 &position,
    const glm::vec2 &size,
    const glm::vec4 &color)
{
	core::Rectangle rec{};
	rec.set_position(position);
	rec.set_size(size);
	draw_rectangle_pro(instance, rec, glm::vec2{0.0f}, 0.0f, color);
}

void SpriteRenderer::draw_rectangle_rec(
    Instance              &instance,
    const core::Rectangle &rec,
    const glm::vec4       &color)
{
	draw_rectangle_pro(instance, rec, glm::vec2{0.0f}, 0.0f, color);
}

void SpriteRenderer::draw_rectangle_pro(
    Instance              &instance,
    const core::Rectangle &rec,
    const glm::vec2       &origin,
    float                  rotation,
    const glm::vec4       &color)
{
	Sprite sprite;
	sprite.texture_id   = instance.get_white_texture_id();
	sprite.texture_size = glm::ivec2{1, 1};
	sprite.color        = color;

	core::Transform parentTransform;
	parentTransform.set_position(glm::vec3{rec.get_position(), 0.f});
	parentTransform.set_heading(glm::angleAxis(rotation, core::global_front_axis));
	parentTransform.set_scale(glm::vec3{rec.get_size(), 0.f});

	core::Transform pivotTransform;
	pivotTransform.set_position((glm::vec3{-origin * rec.get_size(), 1.f}));

	glm::mat4 tm = parentTransform.as_matrix() * pivotTransform.as_matrix();

	draw_sprite(sprite, tm);
}

void SpriteRenderer::draw_texture(
    shared_ptr<Texture> texture,
    int              posX,
    int              posY,
    const ColorRGBA &tint)
{
	draw_texture_v(texture, glm::vec2{posX, posY}, tint);
}

void SpriteRenderer::draw_texture_v(
    shared_ptr<Texture> texture,
    glm::vec2        position,
    const ColorRGBA &tint)
{
	Sprite    sprite    = make_sprite(texture->get_bindless_id(), texture->get_size_2d(), tint);
	glm::mat4 transform = glm::translate(glm::mat4(1.0), glm::vec3(position, 0.0f));
	draw_sprite(sprite, transform);
}
void SpriteRenderer::draw_texture_ex(
    shared_ptr<Texture> texture,
    glm::vec2        position,
    float            rotation,
    float            scale,
    const ColorRGBA &tint)
{
	Sprite    sprite    = make_sprite(texture->get_bindless_id(), texture->get_size_2d(), tint);
	glm::mat4 transform = glm::translate(glm::mat4(1.0), glm::vec3(position, 0.0f));
	transform           = glm::rotate(transform, rotation, glm::vec3(0, 0, 1));
	transform           = glm::scale(transform, glm::vec3(scale));
	draw_sprite(sprite, transform);
}
void SpriteRenderer::draw_texture_rec(
    shared_ptr<Texture>    texture,
    const core::Rectangle &source,
    glm::vec2              position,
    const ColorRGBA       &tint)
{
	Sprite sprite       = make_sprite(texture->get_bindless_id(), texture->get_size_2d(), tint);
	sprite.uv0          = core::pixel_coord_to_uv(glm::vec2(source.x, source.y), sprite.texture_size);
	sprite.uv1          = core::pixel_coord_to_uv(glm::vec2(source.x + source.width, source.y + source.height), sprite.texture_size);
	glm::mat4 transform = glm::translate(glm::mat4(1.0), glm::vec3(position, 0.0f));
	transform           = glm::scale(transform, glm::vec3(source.width, source.height, 1.0));
	draw_sprite(sprite, transform);
}
void SpriteRenderer::draw_texture_pro(
    shared_ptr<Texture>    texture,
    const core::Rectangle &source,
    const core::Rectangle &dest,
    glm::vec2              origin,
    float                  rotation,
    const ColorRGBA       &tint)
{
	Sprite sprite       = make_sprite(texture->get_bindless_id(), texture->get_size_2d(), tint);
	sprite.uv0          = core::pixel_coord_to_uv(glm::vec2(source.x, source.y), sprite.texture_size);
	sprite.uv1          = core::pixel_coord_to_uv(glm::vec2(source.x + source.width, source.y + source.height), sprite.texture_size);
	sprite.pivot        = origin;
	glm::mat4 transform = glm::translate(glm::mat4(1.0), glm::vec3(dest.x, dest.y, 0.0f));
	transform           = glm::rotate(transform, rotation, glm::vec3(0, 0, 1));
	transform           = glm::scale(transform, glm::vec3(source.width, source.height, 1.0));
	draw_sprite(sprite, transform);
}

#pragma endregion

// ---------------------------------------------------------------------------------------------
// gfx::TileMapRenderer
// ---------------------------------------------------------------------------------------------
void TileMapRenderer::draw_level(
    Instance              &instance,
    SpriteRenderer        &renderer,
    const core::LdtkLevel &level)
{
	const auto &layersOpt = level.get_layer_instances();

	if (!layersOpt || layersOpt->empty())
	{
		return;        // nothing to draw
	}

	auto &layers = *layersOpt;
	for (size_t i = 0; i < layers.size(); ++i)
	{
		const auto &layer = layers[layers.size() - 1 - i];        // reverse index;
		if (!layer.get_visible())
		{
			continue;        // toggle visible in ldtk to draw
		}

		draw_layer(instance, renderer, layer);
	}
}

void TileMapRenderer::draw_layer(
    Instance              &instance,
    SpriteRenderer        &renderer,
    const core::LdtkLayer &layer)
{
	glm::vec2 tileSize = {layer.get_grid_size(), layer.get_grid_size()};

	// draw auto-layer tiles
	for (const auto &tile : layer.get_auto_layer_tiles())
	{
		auto texture = instance.get_texture(layer.get_tileset_rel_path().value_or(""));
		WVK_ASSERT_MSG(texture != nullptr, "invalid texture!");
		core::Rectangle rec  = {{WVK_CAST(float, tile.get_src()[0]), WVK_CAST(float, tile.get_src()[1])}, tileSize};

		auto flip = tile.get_f();

		if (flip & 1)
		{
			rec.x += rec.width;
			rec.width *= -1.0f;
		}
		if (flip & 2)
		{
			rec.y += rec.height;
			rec.height *= -1;
		}
		renderer.draw_texture_rec(texture, rec, glm::vec2{tile.get_px()[0], tile.get_px()[1]});
	}
}

UIOverlay::UIOverlay(
    wvk::gfx::Instance &instance,
    GLFWwindow         *window,
    VkFormat            drawImageFormat,
    std::string_view    name)
{
	WVK_ASSERT_MSG(!_is_initialized, "imgui renderer was already initialized!");

	IMGUI_CHECKVERSION();
	_context = ImGui::CreateContext();
	ImGui::SetCurrentContext(_context);

	ImGuiIO &io                          = ImGui::GetIO();
	io.ConfigWindowsMoveFromTitleBarOnly = true;

	io.BackendRendererName = "WEN ImGui";
	io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

	_drawing_pipeline = UIOverlayPipeline(instance, drawImageFormat, name);
	ImGui_ImplGlfw_InitForVulkan(window, true);
	_is_initialized = true;
}

void UIOverlay::cleanup(
    Instance &instance)
{
	WVK_ASSERT_MSG(_is_initialized, "imgui renderer was not initialized!");

	auto device = instance.get_device();

	_drawing_pipeline.cleanup(device);
	ImGui::SetCurrentContext(_context);

	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext(_context);
	_context = nullptr;
}

void UIOverlay::begin_draw()
{
	WVK_ASSERT_MSG(_is_initialized, "imgui renderer was not initialized!");
	ImGui::SetCurrentContext(_context);
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void UIOverlay::end_draw()
{
	WVK_ASSERT_MSG(_is_initialized, "imgui renderer was not initialized!");

	ImGui::SetCurrentContext(_context);
	ImGui::EndFrame();
	ImGui::Render();
}

void UIOverlay::draw(
    VkCommandBuffer cmd,
    Instance       &instance,
    VkImageView     view,
    VkExtent2D      extent)
{
	WVK_ASSERT_MSG(_is_initialized, "imgui renderer was not initialized!");

	ImGui::SetCurrentContext(_context);
	_drawing_pipeline.draw(cmd, instance, view, extent);
}

MeshCache::MeshCache(
    Instance &instance)
{
}

void MeshCache::cleanup()
{
}

MeshId MeshCache::add_mesh(
    Instance &instance,
    CPUMesh  &mesh)
{
	return 0;
}

const GPUMesh &MeshCache::get_gpu_mesh(MeshId id)
{
	return _meshes[id];
}

void MeshCache::upload_mesh(
    Instance                  &instance,
    const shared_ptr<CPUMesh> &cpuMesh,
    const shared_ptr<GPUMesh> &gpuMesh) const
{
}

}        // namespace wvk::gfx
