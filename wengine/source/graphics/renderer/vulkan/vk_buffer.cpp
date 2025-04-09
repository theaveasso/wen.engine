#include "vk_buffer.hpp"

#include "graphics/renderer/vulkan/common/vk_common.hpp"
#include "graphics/renderer/vulkan/common/vk_initializers.hpp"
#include "vk_utils.hpp"

WenVkBuffer wen_vk_buffer_init(
    const WenVkContext *ctx,
    VkDeviceSize        size,
    int32_t             usages,
    int32_t             properties,
    bool                is_lock)
{
	WenVkBuffer buffer;
	buffer.size                         = size;
	buffer.properties                   = properties;
	buffer.usages                       = usages;
	buffer.is_locked                    = is_lock;

	// Create the vertex buffer.
	VkBufferCreateInfo vert_buffer_info = WenVkBufferCreateInfo(
	    buffer.size,
	    buffer.usages);
	vk_check(vkCreateBuffer(
	    ctx->logical_device,
	    &vert_buffer_info,
	    nullptr,
	    &buffer.handle));

	// Get memory requirements.
	VkMemoryRequirements memory_requirements;
	vkGetBufferMemoryRequirements(
	    ctx->logical_device,
	    buffer.handle,
	    &memory_requirements);

	// Allocate memory for the buffer.
	buffer.memory_index = wen_find_memory_type(
	    ctx->physical_device,
	    memory_requirements.memoryTypeBits,
	    buffer.properties);
	VkMemoryAllocateInfo memory_allocate_info = WenVkMemoryAllocateInfo(
	    memory_requirements.size,
	    buffer.memory_index);
	vk_check(vkAllocateMemory(
	    ctx->logical_device,
	    &memory_allocate_info,
	    nullptr,
	    &buffer.memory));

	// Bind the buffer with the allocated memory.
	vk_check(vkBindBufferMemory(
	    ctx->logical_device,
	    buffer.handle,
	    buffer.memory,
	    0));

	// Map the memory and copy the vertex data.
	void *data;
	vk_check(vkMapMemory(
	    ctx->logical_device,
	    buffer.memory,
	    0,
	    buffer.size,
	    0,
	    &data));
	if (buffer.usages & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
	{
		memcpy(
		    data,
		    vertices.data(),
		    WEN_CAST(size_t, buffer.size));
	}
	else if (buffer.usages & VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
	{
		memcpy(
		    data,
		    indices.data(),
		    WEN_CAST(size_t, buffer.size));
	}
	else
	{
		WEN_ERROR("unhandled buffer usage!");
	}
	vkUnmapMemory(
	    ctx->logical_device,
	    buffer.memory);

	return buffer;
}

void wen_vk_buffer_fini(const WenVkContext *ctx, WenVkBuffer *buffer)
{
	vkFreeMemory(ctx->logical_device, buffer->memory, VK_NULL_HANDLE);
	vkDestroyBuffer(ctx->logical_device, buffer->handle, VK_NULL_HANDLE);
}
