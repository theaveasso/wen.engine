#include "hw_game.hpp"

HWGame::HWGame()
{
	_app_config.appName = "Hello world!";
}
void HWGame::on_init()
{
	_draw_image = _instance.create_draw_image(_format, _app_config.renderArea);
	_sprite_renderer.init(_instance, _format);

	_game_camera.init_ortho_2d(WVK_CAST(glm::vec2, get_game_screen_size()));
}
void HWGame::on_update(float dt)
{
}
void HWGame::on_draw()
{
	VkCommandBuffer cmd = _instance.begin_frame();

	wvk::gfx::transition_image(
	    cmd,
	    _draw_image->handle(),
	    VK_IMAGE_LAYOUT_UNDEFINED,
	    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	const glm::vec4 clearColor = glm::vec4{0.1f, 0.1f, 0.1f, 1.0f};

	wvk::gfx::clear(cmd, _draw_image, clearColor);

	_sprite_renderer.begin_draw();

	draw_world();

	_sprite_renderer.draw_rectangle_pro(
	    _instance,
	    wvk::core::Rectangle{glm::vec2{300.f}, glm::vec2{128.f}},
	    glm::vec2{0.0f},
	    0.f,
	    glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});

	_sprite_renderer.end_draw();

	_sprite_renderer.draw(cmd, _instance, _draw_image, _game_camera.get_view_proj());

	wvk::gfx::EndFrameParams endFrameParams{

	};
	_instance.end_frame(cmd, _draw_image, endFrameParams);
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
	// draw tile
	bool has_drew_objects = false;
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
