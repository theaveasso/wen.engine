#include "wvk_gfxinstimpl.hpp"

#include <GLFW/glfw3.h>
#include <imgui.h>

#include "wvk/common/wvk_pch.hpp"
#include "wvk_gfx.hpp"
#include "wvk_gfxtypes.hpp"
#include "wvk_gfxvkinit.hpp"
#include "wvk_gfxvulkan.hpp"

namespace wvk::gfx
{
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

TextureId InstanceImpl::_create_texture_w_pixels(
    VkFormat          format,
    VkImageUsageFlags flags,
    VkExtent2D        extent,
    const void       *data,
    std::string_view  name)
{
	CreateTextureInfo textureInfo = {};
	textureInfo.format            = format;
	textureInfo.usage             = flags;
	textureInfo.extent            = VkExtent3D{extent.width, extent.height, 1};

	VmaAllocationCreateInfo memInfo =
	    init::vma_allocation_create_info();

	shared_ptr<Texture> texture = make_shared<Texture>(
	    device.device,
	    allocator,
	    textureInfo,
	    memInfo,
	    name);

	Buffer stageBuffer = _create_staging_buffer(
	    texture->image_size(),
	    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	    name);

	stageBuffer.copy_data_to_buffer(
	    data,
	    texture->size());

	immediateCommandQueue->submit(device.device, [&](VkCommandBuffer cmd) {
		texture->upload_only(cmd, &stageBuffer);
	});

	TextureId outId = _add_texture(_get_free_texture_id(), texture);

	stageBuffer.cleanup();

	return outId;
}

void InstanceImpl::_create_default_texture()
{
	const uint32_t whitePixel   = 0xFFFFFFFF;
	const uint32_t blackPixel   = 0xFF000000;
	const uint32_t magentaPixel = 0xFF0000FF;

	whiteTextureId = _create_texture_w_pixels(
	    VK_FORMAT_R8G8B8A8_UNORM,
	    VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
	    {.width = 1, .height = 1},
	    &whitePixel);

	std::array<uint32_t, 4> errorPixels{blackPixel, magentaPixel, magentaPixel, blackPixel};

	errorTextureId = _create_texture_w_pixels(
	    VK_FORMAT_R8G8B8A8_UNORM,
	    VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
	    {.width = 2, .height = 2},
	    errorPixels.data());
}

VkCommandBuffer InstanceImpl::_begin_frame()
{
	swapchain->begin_frame(device.device, _get_current_frame_index());
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
	bindlessSetManager->add_image(
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

Buffer InstanceImpl::_create_staging_buffer(
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

	return Buffer(
	    device.device,
	    allocator,
	    &memoryCreateInfo,
	    &bufferInfo,
	    name);
}
}        // namespace wvk::gfx