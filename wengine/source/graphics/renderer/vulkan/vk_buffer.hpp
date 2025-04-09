#pragma once

#include "vk_context.hpp"
#include <vulkan/vulkan.h>

struct WenVkBuffer
{
	VkBuffer       handle = VK_NULL_HANDLE;
	VkDeviceSize   size{};
	VkDeviceMemory memory = VK_NULL_HANDLE;
	uint32_t       memory_index{};
	int32_t        properties{};
	int32_t        usages{};
	bool           is_locked{};
};

/// @brief Initialize buffer
WenVkBuffer wen_vk_buffer_init(
    const WenVkContext *ctx,
    VkDeviceSize        size,
    int32_t             usages,
    int32_t             properties,
    bool                is_lock);

void wen_vk_buffer_fini(
    const WenVkContext *ctx,
    WenVkBuffer        *buffer);
