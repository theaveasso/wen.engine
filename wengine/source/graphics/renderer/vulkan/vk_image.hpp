#pragma once

#include "vk_types.hpp"

WenVkAllocatedImage
vk_allocated_image_init(
    VkDevice device,
    VmaAllocator allocator,
    VkImageUsageFlags image_usage,
    VkImageViewType view_type,
    VkExtent2D extent,
    VkFormat format,
    uint32_t mip_levels,
    uint32_t samples = 1,
    VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
    VmaAllocationCreateFlags allocation_flags = 0
);

void
vk_allocated_image_fini(
    const WenVkContext *ctx,
    VmaAllocator allocator,
    const WenVkAllocatedImage *allocated_image
);

[[nodiscard]] VkImageView
vk_image_view_init(
    VkDevice device,
    VkImage image,
    VkImageViewType image_type,
    VkFormat format,
    VkImageAspectFlags aspect_flags,
    uint32_t mip_levels = 1
);

void
vk_image_view_fini(
    const WenVkContext *ctx,
    VkImageView image_view
);

VkImageSubresourceRange
vk_image_subresource_range_init(
    VkImageAspectFlags aspect_flags,
    uint32_t layer_count,
    uint32_t mip_level,
    uint32_t base_mip_level
);

void
vk_image_blit(
    VkCommandBuffer cmd,
    VkImage src,
    VkImage dst,
    VkExtent2D src_extent,
    VkExtent2D dst_extent,
    uint32_t src_mip_levels = 0,
    uint32_t dst_mip_levels = 0,
    uint32_t src_layer_count = 1,
    uint32_t dst_layer_count = 1
);