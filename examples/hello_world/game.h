#pragma once

#include <wvk/wvk_wen.hpp>

using namespace wvk;

class HWGame : public wvk::Application
{
  public:
	HWGame();

	void on_init() override;
	void on_update(float dt) override;
	void on_draw() override;
	void on_cleanup() override;

	void load_app_settings() override;

	glm::ivec2 get_game_screen_size() const;
	glm::vec2  get_mouse_game_screen_position() const;
	glm::vec2  get_mouse_world_position() const;

	std::shared_ptr<wvk::gfx::Texture> get_main_draw_image() const;

  private:
	VkFormat _format = VK_FORMAT_R16G16B16A16_SFLOAT;

	shared_ptr<gfx::Texture> _draw_texture;
	glm::vec2                _render_size = glm::vec2(1.0f);

	gfx::Scene _current_scene;

	gfx::SpriteRenderer  _sprite_renderer;
	gfx::TileMapRenderer _tilemap_renderer;

	std::unique_ptr<wvk::gfx::DeferredRenderer> _renderer;

	shared_ptr<gfx::MeshCache>     _mesh_cache;
	shared_ptr<gfx::MaterialCache> _material_cache;

	core::Camera _game_camera;

	core::LdtkJson _ldtk_json;

	flecs::world _world;

  private:
	void change_level(std::string_view levelName);
	void do_change_level();

	void on_level_enter();
	void on_level_exit();

	void draw_world();
	void draw_game_objects();
	void draw_ui();
};
