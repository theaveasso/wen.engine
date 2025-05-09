#include "wvk_inst.gfx.hpp"

#include <GLFW/glfw3.h>

#include "wvk_gfx.hpp"
#include "wvk_init.gfx.hpp"
#include "wvk_instimpl.gfx.hpp"
#include "wvk_types.gfx.hpp"
#include "wvk_vulkan.gfx.hpp"

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

	_instance_impl->_check_device_capabilities();

	_instance_impl->_create_vma_allocator();

	int w, h;
	glfwGetWindowSize(window, &w, &h);
	_instance_impl->format = VK_FORMAT_B8G8R8A8_SRGB;

	_instance_impl->swapchain =
	    make_unique<Swapchain>(
	        _instance_impl->device,
	        _instance_impl->format,
	        w, h, _instance_impl->vsync,
	        _instance_impl->appName);

	_instance_impl->immediateCommandQueue =
	    make_unique<ImmediateCommandQueue>(
	        get_device(),
	        _instance_impl->graphicsFamilyQueueIndex,
	        _instance_impl->graphicsQueue);

	_instance_impl->bindlessSetManager =
	    make_unique<BindlessSetManager>(
	        get_device(), 1);

	_instance_impl->_create_default_texture();

	_instance_impl->_create_frame_data();

	_instance_impl->imguiRenderer =
	    make_shared<UIOverlay>(
	        *this,
	        window,
	        _instance_impl->format,
	        "imgui");

	glfwSetCursorPosCallback(window, [](GLFWwindow *window, double x, double y) {
		ImGui::GetIO().MousePos = ImVec2(WVK_CAST(float, x), WVK_CAST(float, y));
	});
	glfwSetMouseButtonCallback(window, [](GLFWwindow *window, int button, int action, int mods) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		const ImGuiMouseButton_ imguiButton =
		    (button == GLFW_MOUSE_BUTTON_LEFT) ? ImGuiMouseButton_Left : (button == GLFW_MOUSE_BUTTON_RIGHT ? ImGuiMouseButton_Right : ImGuiMouseButton_Middle);
		ImGuiIO &io               = ImGui::GetIO();
		io.MousePos               = ImVec2(WVK_CAST(float, xpos), WVK_CAST(float, ypos));
		io.MouseDown[imguiButton] = action == GLFW_PRESS;
	});

	for (size_t i = 0; i < FRAME_OVERLAP; ++i)
	{
		_instance_impl->frames[i].tracyVkCtx =
		    TracyVkContext(_instance_impl->physicalDevice.physical_device,
		                   _instance_impl->device.device,
		                   _instance_impl->graphicsQueue,
		                   _instance_impl->frames[i].primaryCommandBuffer);
	}

	_instance_impl->isInitialized = true;
}

void Instance::cleanup()
{
	WVK_ASSERT_MSG(_instance_impl->isInitialized, "vulkan instance was not initialized!");
	_instance_impl->_destroy_textures();

	for (auto &frame : _instance_impl->frames)
	{
		vkFreeCommandBuffers(get_device(), frame.primaryPool, 1, &frame.primaryCommandBuffer);
		vkDestroyCommandPool(get_device(), frame.primaryPool, VK_NULL_HANDLE);
		TracyVkDestroy(frame.tracyVkCtx);
	}

	_instance_impl->imguiRenderer->cleanup(*this);
	_instance_impl->swapchain->cleanup(*this);
	_instance_impl->immediateCommandQueue->cleanup(*this);

	vkb::destroy_device(_instance_impl->device);
	vkb::destroy_surface(_instance_impl->instance, _instance_impl->surface);
	vkb::destroy_instance(_instance_impl->instance);
}

shared_ptr<Texture> Instance::get_texture(uint32_t index)
{
	return _instance_impl->textures[index];
}

shared_ptr<Texture> Instance::get_texture(std::string_view relPath)
{
	auto it = _instance_impl->tilesetPathToTextureId.find(std::string(relPath));
	if (it != _instance_impl->tilesetPathToTextureId.end())
	{
		TextureId textureId = it->second;
		WVK_ASSERT_MSG(textureId != NULL_TEXTURE_ID, "invalid texture id!");
		if (textureId < _instance_impl->textures.size())
		{
			return _instance_impl->textures[textureId];
		}
	}
	return nullptr;
}

void Instance::register_tileset_texture(std::string_view relPath, TextureId id)
{
	_instance_impl->tilesetPathToTextureId[std::string(relPath)] = id;
}

TextureId Instance::get_white_texture_id()
{
	return _instance_impl->whiteTextureId;
}

TextureId Instance::get_error_texture_id()
{
	return _instance_impl->errorTextureId;
}

void Instance::imgui_begin_frame()
{
	_instance_impl->imguiRenderer->begin_draw();
}

void Instance::imgui_end_frame()
{
	_instance_impl->imguiRenderer->end_draw();
}

ImGuiContext *Instance::get_imgui_ctx()
{
	return _instance_impl->imguiRenderer->get_context();
}

VkCommandBuffer Instance::begin_frame()
{
	return _instance_impl->_begin_frame();
}

void Instance::end_frame(
    VkCommandBuffer           &cmd,
    const shared_ptr<Texture> &drawImage,
    const EndFrameParams      &params)
{
	const auto [imageIndex, swapchainImage] =
	    _instance_impl->swapchain->acquire_image(get_device(), get_current_frame_index());

	if (swapchainImage->handle() == VK_NULL_HANDLE)
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
		blit_image(cmd, drawImage->handle(), swapchainImage->handle(), drawImage->get_extent_2d(), _instance_impl->swapchain->extent(), VK_FILTER_NEAREST);
	}

	{        // draw ImGui
		if (params.drawImGui)
		{
			transition_image(cmd, swapchainImage->handle(), swapchainLayout, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
			swapchainLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			cmd_begin_w_label(cmd, "draw imgui");
			WVK_PROFILER_ZONE("ImGui_draw", WVK_PROFILER_COLOR_CMD_DRAW);
			WVK_PROFILER_GPU_ZONE("ImGui", get_tracy_vk_ctx(), cmd, WVK_PROFILER_COLOR_CMD_DRAW);
			_instance_impl->imguiRenderer->draw(
			    cmd,
			    *this,
			    _instance_impl->swapchain->get_image_view(imageIndex),
			    _instance_impl->swapchain->extent());
			WVK_PROFILER_ZONE_END();
			cmd_end_w_label(cmd);
		}
	}

	{        // prepare for present
		transition_image(cmd, swapchainImage->handle(), swapchainLayout, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		swapchainLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		(void) swapchainLayout;
	}

	{
		TracyVkCollect(get_tracy_vk_ctx(), get_current_frame().primaryCommandBuffer);
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

const TracyVkCtx &Instance::get_tracy_vk_ctx() const
{
	return _instance_impl->_get_current_frame_data().tracyVkCtx;
}

void Instance::recreate_swapchain(uint32_t width, uint32_t height)
{
	WVK_ASSERT_MSG(width != 0 && height != 0, "width and height must not equal 0");

	wait_idle();

	_instance_impl->swapchain->reinit(_instance_impl->device, _instance_impl->format, width, height);
}

VkImageView Instance::get_swapchain_view(uint32_t frameIndex)
{
	return _instance_impl->swapchain->get_image_view(frameIndex);
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
	return _instance_impl->swapchain->extent();
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

shared_ptr<Buffer> Instance::make_persistent_buffer(
    VkDeviceSize       size,
    VkBufferUsageFlags usages,
    std::string_view   name) const
{
	// memory usage: VMA_MEMORY_USAGE_AUTO_PREFER_HOST:
	// Selects best memory type automatically with preference for CPU (host) memory.
	// memory flags:
	// - MAPPED_BIT: Automatically maps the buffer at allocation time.
	// - SEQUENTIAL_WRITE_BIT: Optimized for sequential CPU writes (e.g., memcpy).
	VmaAllocationCreateInfo memoryCreateInfo =
	    init::vma_allocation_create_info(
	        VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
	        0,
	        VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

	// buffer usage:
	// - VK_BUFFER_USAGE_TRANSFER_SRC_BIT: Allows copying to GPU-local buffers.
	// - `usages`: Additional usage flags passed by the caller.
	VkBufferCreateInfo bufferInfo =
	    init::buffer_create_info(
	        size,
	        usages | VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

	return make_shared<Buffer>(
	    get_device(),
	    get_allocator(),
	    &memoryCreateInfo,
	    &bufferInfo,
	    name);
}

shared_ptr<Buffer> Instance::make_gpu_buffer(
    VkDeviceSize       size,
    VkBufferUsageFlags usages,
    std::string_view   name) const
{
	// memory usage:
	// - VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE: Selects best memory type automatically with preference for GPU (device) memory.
	VmaAllocationCreateInfo memoryCreateInfo =
	    init::vma_allocation_create_info(VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);

	// buffer usage:
	// - VK_BUFFER_USAGE_TRANSFER_SRC_BIT: Allows uploading from a staging buffers.
	// - `usages`: Additional usage flags passed by the caller.
	VkBufferCreateInfo bufferInfo =
	    init::buffer_create_info(size, usages | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

	return make_shared<Buffer>(
	    get_device(),
	    get_allocator(),
	    &memoryCreateInfo,
	    &bufferInfo,
	    name);
}

[[nodiscard]] shared_ptr<NBuffer> Instance::make_nbuffer(
    VkDeviceSize       size,
    VkBufferUsageFlags usages,
    std::string_view   name) const
{
	return make_shared<NBuffer>(
	    *this,
	    size,
	    usages,
	    FRAME_OVERLAP,
	    name);
}

void Instance::upload_buffer_to_gpu(
    const shared_ptr<Buffer> &gpuBuffer,
    void           *data,
    VkDeviceSize    totalSize,
    VkDeviceSize              offset) const
{
	// 1-
	auto stageBuffer = make_persistent_buffer(totalSize);

	_instance_impl->immediateCommandQueue->submit(
	    get_device(), [stageBuffer, gpuBuffer, data, totalSize, offset](VkCommandBuffer cmd) {
		    stageBuffer->upload(cmd, gpuBuffer, data, totalSize, offset);
	    });
}

uint32_t Instance::get_current_frame_index() const
{
	return _instance_impl->_get_current_frame_index();
}

wvk::FrameData &Instance::get_current_frame()
{
	return _instance_impl->_get_current_frame_data();
}

[[nodiscard]] TextureId Instance::make_texture_w_pixels(
    VkFormat          format,
    VkImageUsageFlags usages,
    VkExtent2D        extent,
    const void       *data,
    std::string_view  name)
{
	return _instance_impl->_create_texture_w_pixels(
	    format,
	    usages,
	    extent,
	    data,
	    name);
}

TextureId Instance::make_texture_w_path(
    VkFormat                     format,
    VkImageUsageFlags            usages,
    const std::filesystem::path &path,
    std::string_view             name)
{
	ImageData image;
	load_image(path, &image);
	return _instance_impl->_create_texture_w_pixels(
	    format,
	    usages,
	    VkExtent2D(WVK_CAST(uint32_t, image.width), WVK_CAST(uint32_t, image.height)),
	    image.pixels,
	    name);
}

TextureId Instance::make_texture(
    const CreateTextureInfo &createInfo,
    std::string_view         name)
{
	auto texture = make_shared<Texture>(
	    this->get_device(),
	    this->get_allocator(),
	    createInfo,
	    std::nullopt,
	    name);

	auto textureId = _instance_impl->_add_texture(
	    _instance_impl->_get_free_texture_id(),
	    texture);

	return textureId;
}

VkSampleCountFlagBits Instance::get_max_supported_sampling_count()
{
	return _instance_impl->highestSupportedSamples;
}
}        // namespace wvk::gfx
