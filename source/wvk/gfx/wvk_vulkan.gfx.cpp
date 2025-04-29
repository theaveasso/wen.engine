#include "wvk_vulkan.gfx.hpp"

#include <utility>

#include <vk_mem_alloc.h>

#include "common/wvk_error.hpp"
#include "common/wvk_filesystem.hpp"

#include "wvk_init.gfx.hpp"
#include "wvk_inst.gfx.hpp"
#include "wvk_utils.gfx.hpp"

namespace wvk::gfx
{
static constexpr uint32_t MAX_BINDLESS_RESOURCES = 16536;
static constexpr uint32_t MAX_SAMPLERS           = 32;

static constexpr uint32_t TEXTURE_BINDING = 0;
static constexpr uint32_t SAMPLER_BINDING = 1;

BindlessSetManager::BindlessSetManager(
    VkDevice         device,
    float            maxAnisotropy,
    std::string_view name) :
    _debug_name("bindless set manager " + std::string{name})
{
	(void) name;
	const std::array<VkDescriptorPoolSize, 2> poolSizesBindless = {
	    VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, MAX_BINDLESS_RESOURCES},
	    VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_SAMPLER, MAX_SAMPLERS}};

	const VkDescriptorPoolCreateInfo poolInfo =
	    init::descriptor_pool_create_info(
	        MAX_BINDLESS_RESOURCES * WVK_CAST(uint32_t, poolSizesBindless.size()),
	        poolSizesBindless);

	vk_assert(vkCreateDescriptorPool(
	    device,
	    &poolInfo,
	    VK_NULL_HANDLE,
	    &_desc_pool));
	add_debug_w_label(device, _desc_pool, VK_OBJECT_TYPE_DESCRIPTOR_POOL, _debug_name);

	// build descriptor set layout
	const std::array<VkDescriptorSetLayoutBinding, 2> bindings = {
	    init::descriptor_set_layout_binding(TEXTURE_BINDING, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, VK_SHADER_STAGE_ALL, MAX_BINDLESS_RESOURCES),
	    init::descriptor_set_layout_binding(SAMPLER_BINDING, VK_DESCRIPTOR_TYPE_SAMPLER, VK_SHADER_STAGE_ALL, MAX_SAMPLERS),
	};

	VkDescriptorBindingFlags bindlessFlags =
	    VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;

	const std::array<VkDescriptorBindingFlags, 2> bindingFlags = {bindlessFlags, bindlessFlags};

	const VkDescriptorSetLayoutBindingFlagsCreateInfo flagInfo =
	    init::descriptor_set_layout_binding_flags_create_info(bindingFlags);

	const VkDescriptorSetLayoutCreateInfo setLayoutInfo =
	    init::descriptor_set_layout_create_info(&flagInfo, bindings, VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT);

	vk_assert(vkCreateDescriptorSetLayout(
	    device,
	    &setLayoutInfo,
	    VK_NULL_HANDLE,
	    &_desc_set_layout));
	add_debug_w_label(device, _desc_set_layout, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, _debug_name);

	// allocate descriptor set
	const VkDescriptorSetAllocateInfo allocateInfo =
	    init::descriptor_set_allocate_info(_desc_pool, &_desc_set_layout);

	vk_assert(vkAllocateDescriptorSets(device, &allocateInfo, &_desc_set));

	// keep in sync with bindless.glsl
	static constexpr uint32_t NEAREST_SAMPLER_ID = 0;
	static constexpr uint32_t LINEAR_SAMPLER_ID  = 1;
	static constexpr uint32_t SHADOW_SAMPLER_ID  = 2;

	{        // init nearest
		VkSamplerCreateInfo createInfo = {};
		createInfo.sType               = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		createInfo.pNext               = VK_NULL_HANDLE;
		createInfo.magFilter           = VK_FILTER_NEAREST;
		createInfo.minFilter           = VK_FILTER_NEAREST;
		_nearest_sampler =
		    std::make_shared<Sampler>(
		        device,
		        &createInfo,
		        "default nearest");
		add_sampler(device, NEAREST_SAMPLER_ID, _nearest_sampler);
	}

	{        // init linear
		VkSamplerCreateInfo createInfo = {};
		createInfo.sType               = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		createInfo.pNext               = VK_NULL_HANDLE;
		createInfo.magFilter           = VK_FILTER_LINEAR;
		createInfo.minFilter           = VK_FILTER_LINEAR;
		createInfo.mipmapMode          = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		createInfo.anisotropyEnable    = VK_TRUE;
		createInfo.maxAnisotropy       = maxAnisotropy;

		_linear_sampler =
		    std::make_shared<Sampler>(
		        device,
		        &createInfo,
		        "default linear");
		add_sampler(device, LINEAR_SAMPLER_ID, _linear_sampler);
	}

	{        // init shadow
		VkSamplerCreateInfo createInfo = {};
		createInfo.sType               = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		createInfo.pNext               = VK_NULL_HANDLE;
		createInfo.magFilter           = VK_FILTER_LINEAR;
		createInfo.minFilter           = VK_FILTER_LINEAR;
		createInfo.compareEnable       = VK_TRUE;
		createInfo.compareOp           = VK_COMPARE_OP_GREATER_OR_EQUAL;

		_shadow_map_sampler =
		    std::make_shared<Sampler>(
		        device,
		        &createInfo,
		        "default shadow map");
		add_sampler(device, SHADOW_SAMPLER_ID, _shadow_map_sampler);
	}
}

void BindlessSetManager::cleanup(VkDevice device)
{
	_nearest_sampler->cleanup(device);
	_linear_sampler->cleanup(device);
	_shadow_map_sampler->cleanup(device);

	vkDestroyDescriptorSetLayout(device, _desc_set_layout, VK_NULL_HANDLE);
	vkDestroyDescriptorPool(device, _desc_pool, VK_NULL_HANDLE);
}

void BindlessSetManager::add_image(
    VkDevice    device,
    uint32_t    id,
    VkImageView imageView)
{
	WVK_ASSERT_MSG(imageView != VK_NULL_HANDLE, "image view should be valid");

	const VkDescriptorImageInfo imageInfo =
	    init::descriptor_image_info(
	        VK_NULL_HANDLE,
	        imageView,
	        VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL);

	const VkWriteDescriptorSet writeSet =
	    init::write_descriptor_set(
	        VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
	        TEXTURE_BINDING,
	        id,
	        _desc_set,
	        &imageInfo);

	vkUpdateDescriptorSets(device, 1, &writeSet, 0, VK_NULL_HANDLE);
}

void BindlessSetManager::add_sampler(
    VkDevice                        device,
    uint32_t                        id,
    const std::shared_ptr<Sampler> &sampler)
{
	const VkDescriptorImageInfo imageInfo =
	    init::descriptor_image_info(
	        sampler->handle(),
	        VK_NULL_HANDLE,
	        VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL);

	const VkWriteDescriptorSet writeSet =
	    init::write_descriptor_set(
	        VK_DESCRIPTOR_TYPE_SAMPLER,
	        SAMPLER_BINDING,
	        id,
	        _desc_set,
	        &imageInfo);

	vkUpdateDescriptorSets(device, 1, &writeSet, 0, VK_NULL_HANDLE);
}

Swapchain::Swapchain(
    const vkb::Device &device,
    VkFormat           format,
    uint32_t           width,
    uint32_t           height,
    bool               vsync,
    std::string_view   name) :
    _debug_name("swapchain_" + std::string(name)),
    _vsync(vsync)
{
	(void) name;
	WVK_ASSERT_MSG(!_is_initialized, "swapchain was already initialized!");

	// init swapchain
	_swapchain =
	    vkb::SwapchainBuilder{device}
	        .set_desired_format(VkSurfaceFormatKHR{.format = format, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
	        .set_desired_extent(width, height)
	        .set_image_usage_flags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT)
	        .set_desired_present_mode(_vsync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_MAILBOX_KHR)
	        .add_fallback_present_mode(VK_PRESENT_MODE_FIFO_KHR)
	        .build()
	        .value();

	add_debug_w_label(device.device, _swapchain.swapchain, VK_OBJECT_TYPE_SWAPCHAIN_KHR, _debug_name);

	// init sync objects
	const VkFenceCreateInfo fenceInfo =
	    init::fence_create_info();

	const VkSemaphoreCreateInfo semaphoreCreateInfo =
	    init::semaphore_create_info();

	for (size_t i = 0; i < FRAME_OVERLAP; ++i)
	{
		vk_assert(vkCreateFence(device.device, &fenceInfo, VK_NULL_HANDLE, &_frames[i].render_fence));
		vk_assert(vkCreateSemaphore(device.device, &semaphoreCreateInfo, VK_NULL_HANDLE, &_frames[i].swapchain_semaphore));
		vk_assert(vkCreateSemaphore(device.device, &semaphoreCreateInfo, VK_NULL_HANDLE, &_frames[i].render_semaphore));

		add_debug_w_label(device.device, _frames[i].render_fence, VK_OBJECT_TYPE_FENCE, "render fence" + std::to_string(i) + " " + _debug_name);
		add_debug_w_label(device.device, _frames[i].swapchain_semaphore, VK_OBJECT_TYPE_SEMAPHORE, "swapchain semaphore" + std::to_string(i) + " " + _debug_name);
		add_debug_w_label(device.device, _frames[i].render_semaphore, VK_OBJECT_TYPE_SEMAPHORE, "render semaphore" + std::to_string(i) + " " + _debug_name);
	}

	// init swapchain images and imageview
	VkExtent3D imageExtents = {};
	imageExtents.width      = _swapchain.extent.width;
	imageExtents.height     = _swapchain.extent.height;
	imageExtents.depth      = 1;

	std::vector<VkImage> images = _swapchain.get_images().value();
	for (size_t i = 0; i < _swapchain.image_count; ++i)
	{
		_swapchain_images.emplace_back(
		    make_shared<Texture>(
		        device.device,
		        images[i],
		        format,
		        imageExtents,
		        0,
		        0,
		        std::to_string(i) + " " + _debug_name));
	}

	_is_initialized = true;
}

void Swapchain::reinit(
    const vkb::Device &device,
    VkFormat           format,
    uint32_t           width,
    uint32_t           height)
{
	WVK_ASSERT_MSG(_is_initialized, "swapchain was not initialized!");
	WVK_ASSERT_MSG(_swapchain, "failed to reinit, swapchain invalid");
	WVK_ASSERT_MSG(format == VK_FORMAT_B8G8R8A8_SRGB, "swapchain format and support format not match");

	_swapchain =
	    vkb::SwapchainBuilder{device}
	        .set_old_swapchain(_swapchain)
	        .set_desired_extent(width, height)
	        .set_desired_format(VkSurfaceFormatKHR{.format = format, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
	        .set_image_usage_flags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT)
	        .set_desired_present_mode(_vsync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_MAILBOX_KHR)
	        .add_fallback_present_mode(VK_PRESENT_MODE_FIFO_KHR)
	        .build()
	        .value();

	for (auto &texture : _swapchain_images)
	{
		vkDestroyImageView(device.device, texture->get_image_view(), VK_NULL_HANDLE);
	}

	// init swapchain images and imageview
	VkExtent3D imageExtents = {};
	imageExtents.width      = _swapchain.extent.width;
	imageExtents.height     = _swapchain.extent.height;
	imageExtents.depth      = 1;

	std::vector<VkImage> images = _swapchain.get_images().value();
	for (size_t i = 0; i < _swapchain.image_count; ++i)
	{
		_swapchain_images.emplace_back(
		    make_shared<Texture>(
		        device.device,
		        images[i],
		        format,
		        imageExtents,
		        1,
		        1,
		        "image" + std::to_string(i) + " " + std::string(_debug_name)));
	}
}
void Swapchain::cleanup(Instance &instance)
{
	WVK_ASSERT_MSG(_is_initialized, "swapchain was not initialized!");

	auto device = instance.get_device();

	for (auto &texture : _swapchain_images)
	{
		texture->cleanup(
		    instance.get_device(),
		    instance.get_allocator());
	}

	for (size_t i = 0; i < FRAME_OVERLAP; ++i)
	{
		vkDestroyFence(device, _frames[i].render_fence, VK_NULL_HANDLE);
		vkDestroySemaphore(device, _frames[i].swapchain_semaphore, VK_NULL_HANDLE);
		vkDestroySemaphore(device, _frames[i].render_semaphore, VK_NULL_HANDLE);
	}

	vkb::destroy_swapchain(_swapchain);
}

void Swapchain::begin_frame(
    VkDevice device,
    size_t   frameIndex) const
{
	const FrameData &frame = _frames[frameIndex];
	vk_assert(vkWaitForFences(device, 1, &frame.render_fence, VK_TRUE, UINT64_MAX));
}

void Swapchain::resent_fence(
    VkDevice device,
    size_t   frameIndex) const
{
	const FrameData &frame = _frames[frameIndex];
	vk_assert(vkResetFences(device, 1, &frame.render_fence));
}

std::pair<uint32_t, shared_ptr<Texture>> Swapchain::acquire_image(
    VkDevice device,
    size_t   frameIndex)
{
	const VkResult result =
	    vkAcquireNextImageKHR(
	        device,
	        _swapchain.swapchain,
	        UINT64_MAX,
	        _frames[frameIndex].swapchain_semaphore,
	        VK_NULL_HANDLE,
	        &_current_image_index);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		_dirty = true;
	}
	else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to acquire swapchain image!");
	}

	return {_current_image_index, _swapchain_images[_current_image_index]};
}

void Swapchain::submit_and_present(
    VkCommandBuffer &cmd,
    VkQueue          graphicsQueue,
    size_t           frameIndex,
    uint32_t         swapchainImageIndex)
{
	const FrameData &frame = _frames[frameIndex];

	// submit
	const VkCommandBufferSubmitInfo bufferSubmitInfo =
	    init::command_buffer_submit_info(cmd);

	const VkSemaphoreSubmitInfo waitInfo =
	    init::semaphore_submit_info(
	        frame.swapchain_semaphore,
	        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);

	const VkSemaphoreSubmitInfo signalInfo =
	    init::semaphore_submit_info(
	        frame.render_semaphore,
	        VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT);

	const VkSubmitInfo2 submitInfo = init::submit_info_2(
	    &bufferSubmitInfo,
	    &signalInfo,
	    &waitInfo);

	vk_assert(vkQueueSubmit2(
	    graphicsQueue,
	    1,
	    &submitInfo,
	    frame.render_fence));

	// present
	const VkPresentInfoKHR presentInfo =
	    init::present_info(
	        &_swapchain.swapchain,
	        &frame.render_semaphore,
	        &swapchainImageIndex);

	const VkResult result = vkQueuePresentKHR(
	    graphicsQueue,
	    &presentInfo);

	if (result != VK_SUCCESS)
	{
		WVK_ERROR("failed to present");
		_dirty = true;
	}
}

#pragma region vk::Shader
Shader::Shader(
    VkDevice              device,
    VkShaderStageFlagBits stage,
    std::string_view      filename,
    std::string_view      entryPoint,
    std::string_view      name) :
    _debug_name("shader " + std::string{name}),
    _entry_point(entryPoint),
    _stage(stage)
{
	(void) name;
	init(device, filename);
}
void Shader::init(
    VkDevice         device,
    std::string_view filename)
{
	std::vector<uint32_t> code = utils::ReadShaderSpirv(filename);

	const VkShaderModuleCreateInfo shaderModuleInfo =
	    init::shader_module_create_info(code);

	vk_assert(
	    vkCreateShaderModule(
	        device,
	        &shaderModuleInfo,
	        VK_NULL_HANDLE,
	        &_module));

	add_debug_w_label(device, _module, VK_OBJECT_TYPE_SHADER_MODULE, _debug_name);
}
void Shader::cleanup(VkDevice device)
{
	vkDestroyShaderModule(device, _module, VK_NULL_HANDLE);
}
VkShaderModule Shader::handle() const
{
	return _module;
}
VkShaderStageFlagBits Shader::stage() const
{
	return _stage;
}
#pragma endregion

#pragma region gfx::Buffer

Buffer::Buffer(
    VkDevice                  device,
    VmaAllocator              vmaAllocator,
    VmaAllocationCreateInfo  *allocationCreateInfo,
    const VkBufferCreateInfo *bufferInfo,
    std::string_view          name) :
    _debug_name("buffer_" + std::string(name)),
    _allocator(vmaAllocator),
    _size(bufferInfo->size),
    _allocation_create_info(allocationCreateInfo)
{
	(void) name;
	vk_assert(vmaCreateBuffer(
	    vmaAllocator,
	    bufferInfo,
	    _allocation_create_info,
	    &_handle,
	    &_allocation,
	    &_allocation_info));

	if (_allocation_info.pMappedData)
	{
		_mapped_memory = _allocation_info.pMappedData;
	}
	add_debug_w_label(device, _handle, VK_OBJECT_TYPE_BUFFER, _debug_name);

	_is_initialized = true;
}

void Buffer::cleanup()
{
	WVK_ASSERT_MSG(_is_initialized, "buffer was not initialized!");
	if (_mapped_memory && (_allocation_create_info->flags & VMA_ALLOCATION_CREATE_MAPPED_BIT) != 0)
	{
		vmaUnmapMemory(_allocator, _allocation);
	}
	vmaDestroyBuffer(_allocator, _handle, _allocation);
}

void Buffer::upload(
    VkCommandBuffer           cmd,
    const shared_ptr<Buffer> &gpuBuffer,
    void                     *data,
    VkDeviceSize              size,
    VkDeviceSize              offset,
    bool                      sync) const
{
	WVK_ASSERT_MSG(_is_initialized, "buffer was not initialized!");
	WVK_ASSERT_MSG(offset + size <= gpuBuffer->size(), "target size is lesser than upload size");

	if (_size == 0)
	{
		return;
	}

	// sync with previous read
	if (sync)
	{
		const VkBufferMemoryBarrier2 bufferBarrier =
		    init::buffer_memory_barrier2(
		        VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
		        VK_ACCESS_2_MEMORY_READ_BIT,
		        VK_PIPELINE_STAGE_2_TRANSFER_BIT,
		        VK_ACCESS_2_MEMORY_WRITE_BIT,
		        gpuBuffer->handle());

		const VkDependencyInfo depInfo =
		    init::dependency_info({}, {&bufferBarrier, 1});

		vkCmdPipelineBarrier2(cmd, &depInfo);
	}

	copy_data_to_buffer(data, size, offset);

	const VkBufferCopy2 region =
	    init::buffer_copy2(0, offset, size);

	const VkCopyBufferInfo2 copyBufferInfo2 =
	    init::copy_buffer_info2(handle(), gpuBuffer->handle(), &region);

	vkCmdCopyBuffer2(cmd, &copyBufferInfo2);

	if (sync)
	{
		const VkBufferMemoryBarrier2 bufferBarrier =
		    init::buffer_memory_barrier2(
		        VK_PIPELINE_STAGE_2_COPY_BIT,
		        VK_ACCESS_2_MEMORY_WRITE_BIT,
		        VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
		        VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT,
		        gpuBuffer->handle());

		const VkDependencyInfo depInfo =
		    init::dependency_info({}, {&bufferBarrier, 1});

		vkCmdPipelineBarrier2(cmd, &depInfo);
	}
}

void Buffer::copy_data_to_buffer(
    const void  *data,
    VkDeviceSize size,
    VkDeviceSize offset) const
{
	WVK_ASSERT_MSG(_is_initialized, "buffer was not initialized!");
	WVK_ASSERT_MSG(size <= _size, "buffer size must be large or equal the size!");
	WVK_ASSERT_MSG(_mapped_memory != VK_NULL_HANDLE, "buffer memory must be mapped before copying!");
	auto *mappedData = WVK_RCAST(uint8_t *, _mapped_memory);
	memcpy(WVK_CAST(void *, &mappedData[offset]), data, WVK_CAST(size_t, size));
}

VkDeviceAddress Buffer::get_device_address(VkDevice device) const
{
	WVK_ASSERT_MSG(_is_initialized, "buffer was not initialized!");
#if defined(VK_KHR_buffer_device_address) && defined(_WIN32)
	if (!_buffer_device_address)
	{
		VkBufferDeviceAddressInfo bufferDeviceAddress = {};
		bufferDeviceAddress.sType                     = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
		bufferDeviceAddress.buffer                    = _handle;
		_buffer_device_address                        = vkGetBufferDeviceAddress(device, &bufferDeviceAddress);
	}
	return _buffer_device_address;
#else
	return 0;
#endif
}

NBuffer::NBuffer(
    const Instance    &instance,
    VkDeviceSize       size,
    VkBufferUsageFlags usages,
    size_t             numFramesInFlight,
    std::string_view   name) :
    _debug_name("nbuffer " + std::string(name)),
    _frames_in_flight(numFramesInFlight),
    _gpu_buffer_size(size)
{
	WVK_ASSERT_MSG(numFramesInFlight > 0, "");
	WVK_ASSERT_MSG(size > 0, "");

	_gpu_buffer = instance.make_gpu_buffer(size, usages, name);

	_stagings.resize(numFramesInFlight);
	for (size_t i = 0; i < numFramesInFlight; ++i)
	{
		_stagings[i] = instance.make_persistent_buffer(size, usages, name);
	}

	_is_initialized = true;
}

void NBuffer::cleanup()
{
	for (auto &staging : _stagings)
	{
		staging->cleanup();
	}
	_stagings.clear();
	_gpu_buffer->cleanup();
	_is_initialized = false;
}

void NBuffer::upload(
    VkCommandBuffer cmd,
    size_t          frameIndex,
    void           *data,
    VkDeviceSize    size,
    VkDeviceSize    offset,
    bool            sync)
{
	WVK_ASSERT_MSG(_is_initialized, "");
	WVK_ASSERT_MSG(frameIndex < _frames_in_flight, "");
	WVK_ASSERT_MSG(offset + size <= _gpu_buffer_size, "NBuffer::upload: out of bounds write!");

	if (_gpu_buffer_size == 0)
	{
		return;
	}

	// sync with previous read
	if (sync)
	{
		const VkBufferMemoryBarrier2 bufferBarrier =
		    init::buffer_memory_barrier2(
		        VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
		        VK_ACCESS_2_MEMORY_READ_BIT,
		        VK_PIPELINE_STAGE_2_TRANSFER_BIT,
		        VK_ACCESS_2_MEMORY_WRITE_BIT,
		        _gpu_buffer->handle());

		const VkDependencyInfo depInfo =
		    init::dependency_info({}, {&bufferBarrier, 1});

		vkCmdPipelineBarrier2(cmd, &depInfo);
	}

	auto stage = _stagings[frameIndex];
	stage->copy_data_to_buffer(data, size, offset);

	const VkBufferCopy2 region =
	    init::buffer_copy2(offset, offset, _gpu_buffer_size);

	const VkCopyBufferInfo2 copyBufferInfo2 =
	    init::copy_buffer_info2(stage->handle(), _gpu_buffer->handle(), &region);

	vkCmdCopyBuffer2(cmd, &copyBufferInfo2);

	if (sync)
	{
		const VkBufferMemoryBarrier2 bufferBarrier =
		    init::buffer_memory_barrier2(
		        VK_PIPELINE_STAGE_2_COPY_BIT,
		        VK_ACCESS_2_MEMORY_WRITE_BIT,
		        VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
		        VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT,
		        _gpu_buffer->handle());

		const VkDependencyInfo depInfo =
		    init::dependency_info({}, {&bufferBarrier, 1});

		vkCmdPipelineBarrier2(cmd, &depInfo);
	}
}

#pragma endregion
#pragma region vk::Sampler
Sampler::Sampler(
    VkDevice                   device,
    const VkSamplerCreateInfo *createInfo,
    std::string_view           name) :
    _debug_name("sampler " + std::string{name})
{
	(void) name;
	vk_assert(
	    vkCreateSampler(
	        device,
	        createInfo,
	        VK_NULL_HANDLE,
	        &_handle));
	add_debug_w_label(device, _handle, VK_OBJECT_TYPE_SAMPLER, _debug_name);
}
void Sampler::cleanup(VkDevice device)
{
	vkDestroySampler(device, _handle, VK_NULL_HANDLE);
}
VkSampler Sampler::handle() const
{
	return _handle;
}
#pragma endregion

Texture::Texture(
    VkDevice                               device,
    VmaAllocator                           allocator,
    const CreateTextureInfo               &createInfo,
    std::optional<VmaAllocationCreateInfo> customAllocationCreateInfo,
    std::string_view                       name) :
    _debug_name("texture_" + std::string(name)),
    _format(createInfo.format),
    _extent(createInfo.extent),
    _usages(createInfo.usages),
    _own_image(true),
    _base_array_layer(createInfo.baseArrayLayer),
    _mip_levels(createInfo.mipMap),
    _aspect_flags(is_depth() ? VK_IMAGE_ASPECT_DEPTH_BIT : (is_stencil() ? VK_IMAGE_ASPECT_STENCIL_BIT : VK_IMAGE_ASPECT_COLOR_BIT))

{
	(void) name;
	WVK_ASSERT_MSG(createInfo.extent.width > 0 && createInfo.extent.height > 0,
	               "texture dimensions cannot have dimensions equal to 0.");

	static const VmaAllocationCreateInfo defaultAllocInfo =
	    init::vma_allocation_create_info();

	const VmaAllocationCreateInfo allocationCreateInfo =
	    customAllocationCreateInfo.value_or(defaultAllocInfo);

	const VkImageCreateInfo imageInfo =
	    init::image_create_info(
	        _format,
	        _extent,
	        _usages);

	VmaAllocationInfo allocationInfo = {};

	vk_assert(vmaCreateImage(
	    allocator,
	    &imageInfo,
	    &allocationCreateInfo,
	    &_handle,
	    &_allocation,
	    &allocationInfo));

	if (_allocation != VK_NULL_HANDLE)
	{
		VmaAllocationInfo allocation_info;
		vmaGetAllocationInfo(allocator, _allocation, &allocation_info);
		_size = allocationInfo.size;
	}

	bool shouldCreateImageView =
	    ((createInfo.usages & VK_IMAGE_USAGE_SAMPLED_BIT) != 0) ||
	    ((createInfo.usages & VK_IMAGE_USAGE_STORAGE_BIT) != 0) ||
	    ((createInfo.usages & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) != 0) ||
	    ((createInfo.usages & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0);

	if (shouldCreateImageView)
	{
		_image_view = make_image_view(device, _format, createInfo.mipMap, createInfo.baseArrayLayer);
	}
}

Texture::Texture(
    VkDevice         device,
    VkImage          image,
    VkFormat         format,
    VkExtent3D       extent,
    uint32_t         mipMap,
    uint32_t         numLayers,
    std::string_view name) :
    _debug_name("texture_" + std::string(name)),
    _format(format),
    _mip_levels(mipMap),
    _base_array_layer(numLayers),
    _own_image(false),
    _aspect_flags(is_depth() ? VK_IMAGE_ASPECT_DEPTH_BIT : (is_stencil() ? VK_IMAGE_ASPECT_STENCIL_BIT : VK_IMAGE_ASPECT_COLOR_BIT)),
    _extent(extent)
{
	(void) name;
	_handle = image;

	_image_view = make_image_view(device, _format, mipMap, numLayers);
}

void Texture::cleanup(
    VkDevice     device,
    VmaAllocator allocator)
{
	if (_own_image)
	{
		vkDestroyImage(device, _handle, VK_NULL_HANDLE);
	}
	vkDestroyImageView(device, _image_view, VK_NULL_HANDLE);
	vmaDestroyImage(allocator, _handle, _allocation);
}

void Texture::upload(
    VkCommandBuffer          &cmd,
    const shared_ptr<Buffer> &buffer)
{
	WVK_ASSERT_MSG((_usages & VK_IMAGE_USAGE_TRANSFER_DST_BIT) != 0,
	               "image needs to have VK_IMAGE_USAGE_TRANSFER_DST_BIT to upload data to it");

	transition_image(cmd, _handle, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	VkBufferImageCopy bufferCopy               = {};
	bufferCopy.bufferOffset                    = 0;
	bufferCopy.imageExtent                     = _extent;
	bufferCopy.imageSubresource.aspectMask     = _aspect_flags;
	bufferCopy.imageSubresource.layerCount     = 1;
	bufferCopy.imageSubresource.mipLevel       = _mip_levels;
	bufferCopy.imageSubresource.baseArrayLayer = _base_array_layer;

	vkCmdCopyBufferToImage(
	    cmd,
	    buffer->handle(),
	    _handle,
	    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
	    1,
	    &bufferCopy);

	transition_image(
	    cmd,
	    _handle,
	    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
	    VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL);
}

VkImageView Texture::make_image_view(
    VkDevice device,
    VkFormat format,
    uint32_t numMipsLevels,
    uint32_t baseArrayLayer)
{
	_aspect_flags = is_depth() ? VK_IMAGE_ASPECT_DEPTH_BIT : (is_stencil() ? VK_IMAGE_ASPECT_STENCIL_BIT : VK_IMAGE_ASPECT_COLOR_BIT);

	const VkImageSubresourceRange subresource =
	    init::image_subresource_range(
	        _aspect_flags,
	        numMipsLevels,
	        1,
	        1,
	        baseArrayLayer);

	const VkImageViewCreateInfo viewInfo =
	    init::image_view_create_info(format, _handle, subresource);

	VkImageView outImageView = VK_NULL_HANDLE;
	vk_assert(vkCreateImageView(
	    device,
	    &viewInfo,
	    VK_NULL_HANDLE,
	    &outImageView));

	add_debug_w_label(device, outImageView, VK_OBJECT_TYPE_IMAGE_VIEW, _debug_name);

	return outImageView;
}

bool Texture::is_depth() const
{
	return (_format == VK_FORMAT_D16_UNORM || _format == VK_FORMAT_D16_UNORM_S8_UINT ||
	        _format == VK_FORMAT_D24_UNORM_S8_UINT || _format == VK_FORMAT_D32_SFLOAT ||
	        _format == VK_FORMAT_D32_SFLOAT_S8_UINT || _format == VK_FORMAT_X8_D24_UNORM_PACK32);
}

bool Texture::is_stencil() const
{
	return (_format == VK_FORMAT_S8_UINT || _format == VK_FORMAT_D16_UNORM_S8_UINT ||
	        _format == VK_FORMAT_D24_UNORM_S8_UINT || _format == VK_FORMAT_D32_SFLOAT_S8_UINT);
}

ImmediateCommandQueue::ImmediateCommandQueue(
    VkDevice         device,
    uint32_t         graphicsFamilyIndex,
    VkQueue          graphicsQueue,
    std::string_view name) :
    _graphics_queue(graphicsQueue),
    _debug_name("immediate_submit " + std::string{name})
{
	(void) name;
	WVK_ASSERT_MSG(!_is_initialized, "immediate command queue was already initialized!");

	const VkCommandPoolCreateInfo poolInfo =
	    init::command_pool_create_info(graphicsFamilyIndex);

	vk_assert(vkCreateCommandPool(device, &poolInfo, VK_NULL_HANDLE, &_command_pool));

	add_debug_w_label(device, _command_pool, VK_OBJECT_TYPE_COMMAND_POOL, _debug_name);

	const VkCommandBufferAllocateInfo allocateInfo =
	    init::command_buffer_allocate_info(1, _command_pool);

	vk_assert(vkAllocateCommandBuffers(
	    device,
	    &allocateInfo,
	    &_command_buffer));

	add_debug_w_label(device, _command_buffer, VK_OBJECT_TYPE_COMMAND_BUFFER, _debug_name);

	const VkFenceCreateInfo fenceInfo =
	    init::fence_create_info();

	vk_assert(vkCreateFence(
	    device,
	    &fenceInfo,
	    VK_NULL_HANDLE,
	    &_fence));

	add_debug_w_label(device, _fence, VK_OBJECT_TYPE_FENCE, _debug_name);

	_is_initialized = true;
}

void ImmediateCommandQueue::cleanup(Instance &instance)
{
	WVK_ASSERT_MSG(_is_initialized, "immediate command queue was not initialized!");

	auto device = instance.get_device();

	vkFreeCommandBuffers(device, _command_pool, 1, &_command_buffer);
	vkDestroyCommandPool(device, _command_pool, VK_NULL_HANDLE);
	vkDestroyFence(device, _fence, VK_NULL_HANDLE);

	_is_initialized = false;
}
void ImmediateCommandQueue::submit(
    VkDevice                                   device,
    std::function<void(VkCommandBuffer cmd)> &&callback) const
{
	WVK_ASSERT_MSG(_is_initialized, "immediate command queue was not initialized!");
	vk_assert(vkResetFences(device, 1, &_fence));
	vk_assert(vkResetCommandBuffer(_command_buffer, 0));

	const VkCommandBufferBeginInfo beginInfo =
	    init::command_buffer_begin_info();

	vk_assert(vkBeginCommandBuffer(_command_buffer, &beginInfo));
	callback(_command_buffer);
	vk_assert(vkEndCommandBuffer(_command_buffer));

	const VkCommandBufferSubmitInfo bufferSubmitInfo =
	    init::command_buffer_submit_info(_command_buffer);

	const VkSubmitInfo2 submitInfo =
	    init::submit_info_2(&bufferSubmitInfo, VK_NULL_HANDLE, VK_NULL_HANDLE);

	vk_assert(vkQueueSubmit2(_graphics_queue, 1, &submitInfo, _fence));
	vk_assert(vkWaitForFences(device, 1, &_fence, VK_TRUE, UINT64_MAX));
}
#pragma endregion
}        // namespace wvk::gfx
