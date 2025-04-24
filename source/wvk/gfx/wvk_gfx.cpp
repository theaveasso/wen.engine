#include "wvk_gfx.hpp"

#include <GLFW/glfw3.h>
#include <VkBootstrap.h>
#include <backends/imgui_impl_glfw.h>
#include <imgui.h>
#include <volk.h>

#include <glm/gtx/string_cast.hpp>

#include "wvk_gfxinst.hpp"
#include "wvk_gfxinstimpl.hpp"
#include "wvk_gfxtypes.hpp"
#include "wvk_gfxvkinit.hpp"
#include "wvk_gfxvulkan.hpp"

namespace wvk::gfx
{

#pragma region gfx::Sprite
Sprite::Sprite(const std::shared_ptr<Texture> &texture)
{
	set_texture(texture);
}
void Sprite::set_texture(const std::shared_ptr<Texture> &t)
{
	texture      = t;
	texture_id   = t->get_bindless_id();
	texture_size = glm::vec2{t->extent().width, t->extent().height};
}
void Sprite::set_texture_rect(const core::Rectangle &)
{
}
void Sprite::set_pivot_pixel(const glm::ivec2 &pixel)
{
	WVK_ASSERT_MSG(texture_id != Texture::NULL_BINDLESS_ID, "");
	pivot = WVK_CAST(glm::vec2, pixel) / texture_size;
}
#pragma endregion

// ---------------------------------------------------------------------------------------------
// Sprite Renderer
// ---------------------------------------------------------------------------------------------
#pragma region gfx::SpriteRenderer

void SpriteRenderer::init(
    Instance        &instance,
    VkFormat         drawImageFormat,
    std::string_view name)
{
	_sprite_draw_commands.reserve(MAX_SPRITES);
	_drawing_pipeline.init(instance, drawImageFormat, MAX_SPRITES, name);
	_is_initialized = true;
}

void SpriteRenderer::cleanup(Instance &instance)
{
	_drawing_pipeline.cleanup(instance.get_device());
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
    VkCommandBuffer                 cmd,
    Instance                       &instance,
    const std::shared_ptr<Texture> &drawImage)
{
	core::Camera uiCamera;
	draw(cmd, instance, drawImage, uiCamera.get_view_proj());
}

void SpriteRenderer::draw(
    VkCommandBuffer                 cmd,
    Instance                       &instance,
    const std::shared_ptr<Texture> &drawImage,
    const glm::mat4                 viewProjection)
{
	_drawing_pipeline.draw(
	    cmd,
	    instance,
	    drawImage,
	    viewProjection,
	    _sprite_draw_commands);
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
	WVK_ASSERT_MSG(sprite.texture_id != Texture::NULL_BINDLESS_ID, "sprite texture invalid!");

	glm::mat4       transformMatrix = transform;
	const glm::vec2 size            = sprite.texture_size;
	transformMatrix                 = glm::scale(transformMatrix, glm::vec3{size, 1.0f});
	transformMatrix                 = glm::translate(transformMatrix, glm::vec3{-sprite.pivot, 0.f});

	_sprite_draw_commands.push_back(
	    SpriteDrawCommand{
	        .transform = transformMatrix,
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
#pragma endregion

ImGuiRenderer::ImGuiRenderer(
    wvk::gfx::Instance &instance,
    GLFWwindow         *window,
    VkFormat            drawImageFormat,
    std::string_view    name)
{
	_init(instance, window, drawImageFormat, name);
}

void ImGuiRenderer::_init(
    Instance        &instance,
    GLFWwindow      *window,
    VkFormat         drawImageFormat,
    std::string_view name)
{
	WVK_ASSERT_MSG(!_is_initialized, "imgui renderer was already initialized!");

	IMGUI_CHECKVERSION();
	_context = ImGui::CreateContext();
	ImGui::SetCurrentContext(_context);

	ImGuiIO &io                          = ImGui::GetIO();
	io.ConfigWindowsMoveFromTitleBarOnly = true;

	io.BackendRendererName = "WEN ImGui";
	io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

	_drawing_pipeline.init(instance, drawImageFormat, name);
	ImGui_ImplGlfw_InitForVulkan(window, true);
	_is_initialized = true;
}

void ImGuiRenderer::cleanup(
    Instance &instance)
{
	WVK_ASSERT_MSG(_is_initialized, "imgui renderer was not initialized!");
	_drawing_pipeline.cleanup(instance.get_device());
	ImGui::SetCurrentContext(_context);
	ImGui::DestroyContext(_context);
	_context = nullptr;
}

void ImGuiRenderer::begin_draw()
{
	WVK_ASSERT_MSG(_is_initialized, "imgui renderer was not initialized!");
	ImGui::SetCurrentContext(_context);
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void ImGuiRenderer::end_draw()
{
	WVK_ASSERT_MSG(_is_initialized, "imgui renderer was not initialized!");
	WVK_ASSERT_MSG(sizeof(ImDrawIdx) == 2, "the constants below may not work with the ImGui data!");

	ImGui::SetCurrentContext(_context);
	ImGui::EndFrame();
	ImGui::Render();
}

void ImGuiRenderer::draw(
    VkCommandBuffer cmd,
    Instance       &instance,
    VkImageView     view,
    VkExtent2D      extent)
{
	WVK_ASSERT_MSG(_is_initialized, "imgui renderer was not initialized!");

	ImGui::SetCurrentContext(_context);
	_drawing_pipeline.draw(cmd, instance, view, extent);
}
}        // namespace wvk::gfx
