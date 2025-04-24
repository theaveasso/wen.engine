#pragma once

#include <VkBootstrap.h>
#include <glm/mat4x4.hpp>

#include "wvk/common/wvk_defines.hpp"
#include "wvk/common/wvk_pch.hpp"
#include "wvk/core/wvk_core.hpp"

#include "wvk_gfxpipeline.hpp"
#include "wvk_gfxtypes.hpp"
#include "wvk_gfxvulkan.hpp"

struct GLFWwindow;

namespace wvk::gfx
{

// ---------------------------------------------------------------------------------------------
// gfx::Sprite
// ---------------------------------------------------------------------------------------------
#pragma region gfx::Sprite
struct WVK_API Sprite
{
  public:
	Sprite() = default;
	explicit Sprite(const std::shared_ptr<Texture> &texture);

	std::shared_ptr<Texture> texture;
	uint32_t                 texture_id = 0;
	glm::vec2                texture_size;

	glm::vec2 uv0 = glm::vec2{0.f};
	glm::vec2 uv1 = glm::vec2{1.f};

	glm::vec2 pivot = glm::vec2{0.f};
	glm::vec4 color = glm::vec4{1.f, 1.f, 1.f, 1.f};

	void set_texture(const std::shared_ptr<Texture> &texture);
	void set_texture_rect(const core::Rectangle &rec);
	void set_pivot_pixel(const glm::ivec2 &pixel);
};
#pragma endregion

// ---------------------------------------------------------------------------------------------
// gfx::SpriteRenderer
// ---------------------------------------------------------------------------------------------
#pragma region gfx::SpriteRenderer
class WVK_API SpriteRenderer
{
  public:
	static constexpr uint32_t SPRITE_SHADER_ID  = 0;
	static constexpr uint32_t TEXTURE_SHADER_ID = 1;

  public:
	void init(Instance &instance, VkFormat drawImageFormat, std::string_view name = "");
	void cleanup(Instance &instance);

	void begin_draw();
	void end_draw();

	void draw(VkCommandBuffer cmd, Instance &instance, const std::shared_ptr<Texture> &drawImage);
	void draw(VkCommandBuffer cmd, Instance &instance, const std::shared_ptr<Texture> &drawImage, const glm::mat4 viewProjection);

	void draw_sprite(const Sprite &sprite, const glm::vec2 &position, float rotation, const glm::vec2 &scale, uint32_t shaderId = SPRITE_SHADER_ID);

	void draw_sprite(const Sprite &sprite, const glm::mat4 &transform, uint32_t shaderId = SPRITE_SHADER_ID);

	void draw_text();        // TODO

	void draw_rectangle(Instance &instance, int posX, int posY, int width, int height, const glm::vec4 &color);
	void draw_rectangle_v(Instance &instance, const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color);
	void draw_rectangle_rec(Instance &instance, const core::Rectangle &rec, const glm::vec4 &color);
	void draw_rectangle_pro(Instance &instance, const core::Rectangle &rec, const glm::vec2 &origin, float rotation, const glm::vec4 &color);

  private:
	bool _is_initialized = false;

	SpriteRenderingPipeline _drawing_pipeline;

	static const size_t            MAX_SPRITES           = 25000;
	std::vector<SpriteDrawCommand> _sprite_draw_commands = {};
};
#pragma endregion

// ---------------------------------------------------------------------------------------------
// gfx::ImGuiRenderer
// ---------------------------------------------------------------------------------------------
class WVK_API ImGuiRenderer
{
  public:
	explicit ImGuiRenderer(Instance &instance, GLFWwindow *window, VkFormat drawImageFormat, std::string_view name = "");

	void cleanup(VkDevice device);

	void begin_draw();
	void end_draw();
	void draw(VkCommandBuffer cmd, Instance &instance, VkImageView view, VkExtent2D extent);

	ImGuiContext *get_context() { return _context; }

  private:
	bool _is_initialized = false;

	ImGuiContext *_context;

	void _init(Instance &instance, GLFWwindow *window, VkFormat drawImageFormat, std::string_view name = "");

	ImGuiRenderingPipeline _drawing_pipeline;
};
}        // namespace wvk::gfx
