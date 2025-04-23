#include "wvk_gfx.hpp"

#include <GLFW/glfw3.h>
#include <VkBootstrap.h>
#include <vk_mem_alloc.h>
#include <volk.h>

#include <glm/gtx/string_cast.hpp>

#include "wvk_gfximpl.hpp"
#include "wvk_gfxtypes.hpp"
#include "wvk_gfxutils.hpp"
#include "wvk_gfxvkinit.hpp"
#include "wvk_gfxvulkan.hpp"

namespace wvk::gfx
{
Instance::Instance() :
    _instance_impl(std::make_unique<InstanceImpl>())
{
}
Instance::~Instance() = default;

Instance::Instance::Instance(Instance &&) noexcept  = default;
Instance &Instance::operator=(Instance &&) noexcept = default;

void Instance::init(GLFWwindow *window, const char *name, bool vsync)
{
	WVK_ASSERT_MSG(!_instance_impl->isInitialized, "vulkan instance was already initialized!");

	_instance_impl->appName = name;
	_instance_impl->vsync   = vsync;

	_instance_impl->_vulkan_init(window);

	int w, h;
	glfwGetWindowSize(window, &w, &h);
	_instance_impl->format = VK_FORMAT_B8G8R8A8_SRGB;

	_instance_impl->swapchain =
	    new Swapchain(
	        _instance_impl->device,
	        _instance_impl->format,
	        w, h, _instance_impl->vsync,
	        _instance_impl->appName);

	_instance_impl->immediateCommandQueue =
	    new ImmediateCommandQueue(
	        get_device(),
	        _instance_impl->graphicsFamilyQueueIndex,
	        _instance_impl->graphicsQueue);

	_instance_impl->bindlessSetManager =
	    new BindlessSetManager(
	        get_device(), 1);

	_instance_impl->_create_vma_allocator();

	_instance_impl->_create_frame_data();

	_instance_impl->_create_default_texture();

	_instance_impl->isInitialized = true;
}

void Instance::cleanup()
{
	WVK_ASSERT_MSG(_instance_impl->isInitialized, "vulkan instance was not initialized!");
	_instance_impl->_destroy_textures();

	for (size_t i = 0; i < FRAME_OVERLAP; ++i)
	{
		vkFreeCommandBuffers(_instance_impl->device.device, _instance_impl->frames[i].primaryPool, 1, &_instance_impl->frames[i].primaryCommandBuffer);
		vkDestroyCommandPool(_instance_impl->device.device, _instance_impl->frames[i].primaryPool, VK_NULL_HANDLE);
	}

	_instance_impl->swapchain->cleanup(_instance_impl->device.device);
	vkb::destroy_device(_instance_impl->device);
	vkb::destroy_surface(_instance_impl->instance, _instance_impl->surface);
	vkb::destroy_instance(_instance_impl->instance);
}

TextureId Instance::get_white_texture_id()
{
	return _instance_impl->whiteTextureId;
}

TextureId Instance::get_error_texture_id()
{
	return _instance_impl->errorTextureId;
}

VkCommandBuffer Instance::begin_frame()
{
	return _instance_impl->_begin_frame();
}

void Instance::end_frame(
    VkCommandBuffer                 cmd,
    const std::shared_ptr<Texture> &drawImage,
    const EndFrameParams & /* params */)
{
	const auto [imageIndex, swapchainImage] =
	    _instance_impl->swapchain->acquire_image(get_device(), get_current_frame_index());

	if (swapchainImage == VK_NULL_HANDLE)
	{
		return;
	}

	_instance_impl->swapchain->resent_fence(get_device(), get_current_frame_index());

	VkImageLayout swapchainLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	(void) swapchainLayout;

	{        // clear swapchain image
		clear(cmd, swapchainImage, glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});
		transition_image(cmd, swapchainImage->handle(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		swapchainLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	}

	{        // blit from draw to swapchain image
		transition_image(cmd, drawImage->handle(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
		blit_image(cmd, drawImage->handle(), swapchainImage->handle(), drawImage->extent(), _instance_impl->swapchain->swapchain_extent(), VK_FILTER_NEAREST);
	}

	{        // prepare for present
		transition_image(cmd, swapchainImage->handle(), swapchainLayout, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		swapchainLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		(void) swapchainLayout;
	}

	vk_assert(vkEndCommandBuffer(cmd));

	_instance_impl->swapchain->submit_and_present(
	    cmd,
	    _instance_impl->graphicsQueue,
	    get_current_frame_index(),
	    imageIndex);

	_instance_impl->frameNumber++;
}
void Instance::wait_idle() const
{
	vkDeviceWaitIdle(_instance_impl->device.device);
}
void Instance::recreate_swapchain(uint32_t width, uint32_t height)
{
	WVK_ASSERT_MSG(width != 0 && height != 0, "width and height must not equal 0");

	wait_idle();

	_instance_impl->swapchain->reinit(_instance_impl->device, _instance_impl->format, width, height);
}
bool Instance::required_swapchain_reinit() const
{
	return _instance_impl->swapchain->required_reinit();
}
VkDevice Instance::get_device() const
{
	return _instance_impl->device.device;
}
VmaAllocator Instance::get_allocator() const
{
	return _instance_impl->allocator;
}
VkExtent2D Instance::swapchain_extent() const
{
	return _instance_impl->swapchain->swapchain_extent();
}
VkDescriptorSetLayout Instance::get_bindless_desc_set_layout() const
{
	return _instance_impl->bindlessSetManager->get_desc_set_layout();
}
void Instance::bind_bindless_desc_set(VkCommandBuffer cmd, VkPipelineLayout layout) const
{
	vkCmdBindDescriptorSets(
	    cmd,
	    VK_PIPELINE_BIND_POINT_GRAPHICS,
	    layout,
	    0,
	    1,
	    &_instance_impl->bindlessSetManager->get_desc_set(),
	    0,
	    VK_NULL_HANDLE);
}
const VkDescriptorSet &Instance::get_bindless_desc_set() const
{
	return _instance_impl->bindlessSetManager->get_desc_set();
}
std::shared_ptr<Buffer> Instance::create_buffer(
    VkDeviceSize       size,
    VkBufferUsageFlags usages,
    VmaMemoryUsage     memoryUsage,
    std::string_view   name) const
{
	VmaAllocationCreateInfo memoryCreateInfo =
	    init::vma_allocation_create_info(
	        VMA_MEMORY_USAGE_AUTO,
	        memoryUsage,
	        VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT,
	        VK_MEMORY_PROPERTY_HOST_CACHED_BIT);

	VkBufferCreateInfo bufferInfo =
	    init::buffer_create_info(
	        size,
	        usages);

	return std::make_shared<Buffer>(
	    get_device(),
	    get_allocator(),
	    &memoryCreateInfo,
	    &bufferInfo,
	    name);
}
std::shared_ptr<Buffer> Instance::create_persistent_buffer(
    VkDeviceSize       size,
    VkBufferUsageFlags usages,
    std::string_view   name) const
{
	VmaAllocationCreateInfo memoryCreateInfo =
	    init::vma_allocation_create_info(
	        VMA_MEMORY_USAGE_CPU_TO_GPU,
	        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT,        // cpu visible
	        VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

	VkBufferCreateInfo bufferInfo =
	    init::buffer_create_info(
	        size,
	        usages);

	return std::make_shared<Buffer>(
	    get_device(),
	    get_allocator(),
	    &memoryCreateInfo,
	    &bufferInfo,
	    name);
}
std::shared_ptr<Buffer> Instance::create_staging_buffer(
    VkDeviceSize       size,
    VkBufferUsageFlags usages,
    Buffer            *actualBuffer,
    std::string_view   name) const
{
	return std::make_shared<Buffer>(
	    get_device(),
	    get_allocator(),
	    size,
	    usages,
	    actualBuffer,
	    name);
}
std::shared_ptr<Buffer> Instance::create_staging_buffer(
    VkDeviceSize       size,
    VkBufferUsageFlags usages,
    std::string_view   name) const
{
	return _instance_impl->_create_staging_buffer(size, usages, name);
}
VkDeviceAddress Instance::get_buffer_address(
    const std::shared_ptr<Buffer> &buffer) const
{
	return buffer->device_address(get_device());
}
void Instance::destroy_buffer(std::shared_ptr<Buffer> &buffer)
{
	buffer->cleanup();
}
void Instance::upload_buffer_to_gpu(
    VkCommandBuffer cmd,
    Buffer         *gpuBuffer,
    const void     *data,
    VkDeviceSize    totalSize,
    uint32_t        offset) const
{
	std::shared_ptr<Buffer> staging = create_staging_buffer(totalSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, gpuBuffer, "staging buffer");
	staging->copy_data_to_buffer(data, totalSize);
	staging->upload_staging_buffer_to_gpu(cmd, 0, offset);
}

uint32_t Instance::get_current_frame_index() const
{
	return _instance_impl->_get_current_frame_index();
}

std::shared_ptr<Texture> Instance::create_draw_image(
    VkFormat         format,
    glm::ivec2       size,
    std::string_view name) const
{
	VkImageUsageFlags usages = 0;
	usages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	usages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	usages |= VK_IMAGE_USAGE_SAMPLED_BIT;
	usages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	CreateImageInfo createInfo = {};
	createInfo.format          = format;
	createInfo.extents         = VkExtent3D{.width = WVK_CAST(uint32_t, size.x), .height = WVK_CAST(uint32_t, size.y), .depth = 1};
	createInfo.usage           = usages;

	std::string label = "draw image" + std::string(name);
	return std::make_shared<Texture>(
	    get_device(),
	    get_allocator(),
	    createInfo,
	    std::nullopt,
	    label);
}

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
}        // namespace wvk::gfx