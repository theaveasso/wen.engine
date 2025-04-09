#pragma once

#include "privates/wen_pch.hpp"
#include <vulkan/vulkan.h>

/// @brief Initialize VkBufferCreateInfo.
inline VkBufferCreateInfo WenVkBufferCreateInfo(
    VkDeviceSize size,
    int32_t      usage)
{
	VkBufferCreateInfo create_info = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
	create_info.flags              = 0;
	create_info.size               = size;
	create_info.usage              = usage;
	create_info.sharingMode        = VK_SHARING_MODE_EXCLUSIVE;
	return create_info;
}

/// @brief Initialize VkMemoryAllocateInfo.
inline VkMemoryAllocateInfo WenVkMemoryAllocateInfo(
    VkDeviceSize size,
    uint32_t     memory_type_index)
{
	VkMemoryAllocateInfo memory_allocation_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
	memory_allocation_info.allocationSize       = size;
	memory_allocation_info.memoryTypeIndex      = memory_type_index;
	return memory_allocation_info;
}

/// @brief Initialize VkMappedMemoryRange.
inline VkMappedMemoryRange mapped_memory_range()
{
	VkMappedMemoryRange mapped_memory_range = {VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE};
	return mapped_memory_range;
}

/// @brief Initialize VkCommandBufferAllocateInfo.
inline VkCommandBufferAllocateInfo command_buffer_allocate_info(
    VkCommandPool        command_pool,
    VkCommandBufferLevel level,
    uint32_t             buffer_count)
{
	VkCommandBufferAllocateInfo info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
	info.commandPool                 = command_pool;
	info.level                       = level;
	info.commandBufferCount          = buffer_count;
	return info;
};

//-----------------------------------------------------------------------
// Vulkan Pipeline
//-----------------------------------------------------------------------

/// @brief Initialize VkPipelineRenderingCreateInfo, e.g. for use with dynamic rendering extension.
inline VkPipelineRenderingCreateInfo WenPipelineRenderingCreateInfo(
    uint32_t color_attachment_count,
    VkFormat color_attachments)
{
	VkPipelineRenderingCreateInfo create_info = {VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO};
	create_info.pNext                         = VK_NULL_HANDLE;
	create_info.colorAttachmentCount          = color_attachment_count;
	create_info.pColorAttachmentFormats       = &color_attachments;
	//	create_info.depthAttachmentFormat         = VK_FORMAT_D32_SFLOAT;

	return create_info;
}