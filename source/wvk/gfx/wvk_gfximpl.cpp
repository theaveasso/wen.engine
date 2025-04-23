#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <VkBootstrap.h>
#include <vk_mem_alloc.h>
#include <volk.h>

#include "wvk/common/wvk_pch.hpp"
#include "wvk_gfxvkinit.hpp"
#include "wvk_gfxvulkan.hpp"
#include "wvk_gfxtypes.hpp"

namespace wvk::gfx
{
struct InstanceImpl
{
	std::string appName;

	vkb::Instance         instance       = {};
	vkb::PhysicalDevice   physicalDevice = {};
	vkb::Device           device         = {};
	VmaAllocator          allocator      = VK_NULL_HANDLE;
	VkFormat              format         = VK_FORMAT_B8G8R8A8_SRGB;
	VkSurfaceKHR          surface        = VK_NULL_HANDLE;
	Swapchain             swapchain;
	ImmediateCommandQueue immediateCommandQueue;
	BindlessSetManager    bindlessSetManager;

	std::array<FrameData, FRAME_OVERLAP> frames;
	uint32_t                             frameNumber = 0;

	VkQueue  graphicsQueue            = VK_NULL_HANDLE;
	uint32_t graphicsFamilyQueueIndex = 0;

	bool vsync         = false;
	bool isInitialized = false;

	void _vulkan_init(GLFWwindow *window);
	void _create_vma_allocator();
	void _create_frame_data();
	void _create_default_texture();

	std::shared_ptr<Buffer> _create_staging_buffer(VkDeviceSize size, VkBufferUsageFlags usages, std::string_view name = "");

	VkCommandBuffer _begin_frame();

	// images
	TextureVec                     textures = {};
	[[nodiscard]] inline TextureId _get_free_texture_id() const { return WVK_CAST(uint32_t, textures.size()); }
	TextureId                      _add_texture(TextureId id, const std::shared_ptr<Texture> &texture);
	void                           _destroy_textures();

	FrameData             &_get_current_frame_data() { return frames[_get_current_frame_index()]; }
	[[nodiscard]] uint32_t _get_current_frame_index() const { return frameNumber % FRAME_OVERLAP; }

	TextureId whiteTextureId;
	TextureId errorTextureId;
};

void InstanceImpl::_vulkan_init(
    GLFWwindow *window)
{
	vk_assert(volkInitialize());

	instance =
	    vkb::InstanceBuilder{}
	        .set_app_name(appName.c_str())
	        .set_engine_name("WENVK Engine")
	        .request_validation_layers(true)
	        .use_default_debug_messenger()
	        .set_debug_messenger_type(VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
	        .set_debug_messenger_severity(VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
	        .require_api_version(1, 3, 0)
	        .build()
	        .value();

	if (instance == VK_NULL_HANDLE)
	{
		throw std::runtime_error("failed to create vulkan instance.");
	}

	volkLoadInstance(instance);

	const VkResult surfaceResult = glfwCreateWindowSurface(instance, window, VK_NULL_HANDLE, &surface);
	if (surfaceResult != VK_SUCCESS)
	{
		const char *errDesc;
		WVK_ERROR("failed to create vulkan surface", glfwGetError(&errDesc));
		return;
	}

	VkPhysicalDeviceVulkan13Features features_1_3 = {};
	features_1_3.sType                            = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
	features_1_3.synchronization2                 = VK_TRUE;
	features_1_3.dynamicRendering                 = VK_TRUE;

	VkPhysicalDeviceVulkan12Features features_1_2             = {};
	features_1_2.sType                                        = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
	features_1_2.pNext                                        = &features_1_3;
	features_1_2.bufferDeviceAddress                          = VK_TRUE;
	features_1_2.runtimeDescriptorArray                       = VK_TRUE;
	features_1_2.descriptorBindingPartiallyBound              = VK_TRUE;
	features_1_2.descriptorIndexing                           = VK_TRUE;
	features_1_2.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
	features_1_2.descriptorBindingStorageImageUpdateAfterBind = VK_TRUE;
	features_1_2.descriptorBindingPartiallyBound              = VK_TRUE;
	features_1_2.descriptorBindingVariableDescriptorCount     = VK_TRUE;

	VkPhysicalDeviceFeatures2 features_2 = {};
	features_2.sType                     = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	features_2.pNext                     = &features_1_2;

	VkPhysicalDeviceFeatures feature_1 = {};
	feature_1.imageCubeArray           = VK_TRUE;
	feature_1.geometryShader           = VK_TRUE;
	feature_1.depthClamp               = VK_TRUE;
	feature_1.samplerAnisotropy        = VK_TRUE;

	physicalDevice =
	    vkb::PhysicalDeviceSelector{instance}
	        .set_minimum_version(1, 3)
	        .prefer_gpu_device_type(vkb::PreferredDeviceType::discrete)
	        .set_required_features(feature_1)
	        .set_required_features_12(features_1_2)
	        .set_required_features_13(features_1_3)
	        .require_dedicated_transfer_queue()
	        .set_surface(surface)
	        .select()
	        .value();

	if (physicalDevice.physical_device == VK_NULL_HANDLE)
	{
		throw std::runtime_error("failed to select gpu device.");
	}

	device =
	    vkb::DeviceBuilder{physicalDevice}
	        .build()
	        .value();

	if (device.device == VK_NULL_HANDLE)
	{
		throw std::runtime_error("failed to create vulkan logical device");
	}

	graphicsQueue            = device.get_queue(vkb::QueueType::graphics).value();
	graphicsFamilyQueueIndex = device.get_queue_index(vkb::QueueType::graphics).value();
}

void InstanceImpl::_create_vma_allocator()
{
	const auto vulkanFunctions = VmaVulkanFunctions{
	    .vkGetInstanceProcAddr = vkGetInstanceProcAddr,
	    .vkGetDeviceProcAddr   = vkGetDeviceProcAddr};

	VmaAllocatorCreateInfo createInfo =
	    init::vma_allocator_create_info(
	        instance.instance,
	        physicalDevice.physical_device,
	        device.device,
	        &vulkanFunctions);

	vk_assert(vmaCreateAllocator(&createInfo, &allocator));
}

void InstanceImpl::_create_frame_data()
{
	VkCommandPoolCreateInfo poolInfo =
	    init::command_pool_create_info(graphicsFamilyQueueIndex);

	for (size_t i = 0; i < FRAME_OVERLAP; ++i)
	{
		vk_assert(
		    vkCreateCommandPool(device.device, &poolInfo, VK_NULL_HANDLE, &frames[i].primaryPool));

		VkCommandBufferAllocateInfo bufferInfo =
		    init::command_buffer_allocate_info(1, frames[i].primaryPool);

		vk_assert(
		    vkAllocateCommandBuffers(device.device, &bufferInfo, &frames[i].primaryCommandBuffer));

		add_debug_w_label(device, frames[i].primaryPool, VK_OBJECT_TYPE_COMMAND_POOL, appName);
		add_debug_w_label(device, frames[i].primaryCommandBuffer, VK_OBJECT_TYPE_COMMAND_BUFFER, appName);
	}
}

void InstanceImpl::_create_default_texture()
{
	const uint32_t whitePixel   = 0xFFFFFFFF;
	const uint32_t blackPixel   = 0xFF000000;
	const uint32_t magentaPixel = 0xFF0000FF;

	// white texture
	CreateImageInfo whiteTextureInfo = {};
	whiteTextureInfo.format          = VK_FORMAT_R8G8B8A8_UNORM;
	whiteTextureInfo.usage           = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	whiteTextureInfo.extents         = {.width = 1, .height = 1, .depth = 1};

	VmaAllocationCreateInfo memInfo =
	    init::vma_allocation_create_info();

	std::shared_ptr<Texture> whiteTexture = std::make_shared<Texture>(
	    device.device,
	    allocator,
	    whiteTextureInfo,
	    memInfo,
	    "white texture");

	// error texture
	CreateImageInfo errorTextureInfo = {};
	errorTextureInfo.format          = VK_FORMAT_R8G8B8A8_UNORM;
	errorTextureInfo.usage           = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	errorTextureInfo.extents         = {.width = 2, .height = 2, .depth = 1};

	std::array<uint32_t, 4> errorPixels{blackPixel, magentaPixel, magentaPixel, blackPixel};

	std::shared_ptr<Texture> errorTexture = std::make_shared<Texture>(
	    device.device,
	    allocator,
	    errorTextureInfo,
	    memInfo,
	    "error texture");

	std::shared_ptr<Buffer> whiteTextureStagingBuffer =
	    _create_staging_buffer(
	        whiteTexture->image_size(),
	        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	        "white texture upload staging buffer");

	whiteTextureStagingBuffer->copy_data_to_buffer(
	    &whitePixel,
	    whiteTexture->pixel_size_in_bytes() * whiteTexture->extent3d().width * whiteTexture->extent3d().height * whiteTexture->extent3d().depth);

	std::shared_ptr<Buffer> errorTextureStagingBuffer =
	    _create_staging_buffer(
	        errorTexture->image_size(),
	        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	        "error texture upload staging buffer");

	errorTextureStagingBuffer->copy_data_to_buffer(
	    errorPixels.data(),
	    errorTexture->pixel_size_in_bytes() * errorTexture->extent3d().width * errorTexture->extent3d().height * errorTexture->extent3d().depth);

	immediateCommandQueue.submit(device.device, [&](VkCommandBuffer cmd) {
		whiteTexture->upload_only(cmd, whiteTextureStagingBuffer.get());
		errorTexture->upload_only(cmd, errorTextureStagingBuffer.get());
	});

	whiteTextureId = _add_texture(_get_free_texture_id(), whiteTexture);
	errorTextureId = _add_texture(_get_free_texture_id(), errorTexture);

	whiteTextureStagingBuffer->cleanup();
	errorTextureStagingBuffer->cleanup();
}

VkCommandBuffer InstanceImpl::_begin_frame()
{
	swapchain.begin_frame(device.device, _get_current_frame_index());
	const FrameData       &frame = _get_current_frame_data();
	const VkCommandBuffer &cmd   = frame.primaryCommandBuffer;

	const VkCommandBufferBeginInfo beginInfo =
	    init::command_buffer_begin_info();

	vk_assert(
	    vkBeginCommandBuffer(
	        cmd,
	        &beginInfo));

	return cmd;
}

TextureId InstanceImpl::_add_texture(
    TextureId                       id,
    const std::shared_ptr<Texture> &texture)
{
	texture->set_bindless_id(WVK_CAST(uint32_t, id));
	if (id != textures.size())
	{
		textures[id] = texture;
	}
	else
	{
		textures.push_back(texture);
	}
	bindlessSetManager.add_image(
	    device.device,
	    id,
	    texture->image_view());

	return id;
}

void InstanceImpl::_destroy_textures()
{
	for (auto &t : textures)
	{
		t->cleanup(device.device);
	}
}

std::shared_ptr<Buffer> InstanceImpl::_create_staging_buffer(
    VkDeviceSize       size,
    VkBufferUsageFlags usages,
    std::string_view   name)
{
	VmaAllocationCreateInfo memoryCreateInfo =
	    init::vma_allocation_create_info(
	        VMA_MEMORY_USAGE_CPU_ONLY,
	        0,
	        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT);

	VkBufferCreateInfo bufferInfo =
	    init::buffer_create_info(
	        size,
	        usages);

	return std::make_shared<Buffer>(
	    device.device,
	    allocator,
	    &memoryCreateInfo,
	    &bufferInfo,
	    name);
}
}        // namespace wvk::gfx
