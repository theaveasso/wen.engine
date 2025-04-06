#pragma once

#include "privates/wen_pch.hpp"
#include "vk_context.hpp"

struct WenVkAllocatedImage {
    VkImage       image{};
    VkImageView   view{};
    VkExtent3D    extent3d{};
    VkFormat      format{};
    uint32_t      mip_levels{1};
    VmaAllocation allocation{};
};

WenVkAllocatedImage
vk_allocated_image_init(
    VkDevice          device,
    VmaAllocator      allocator,
    VkImageUsageFlags image_usage,
    VkImageViewType   view_type,
    VkExtent2D        extent,
    VkFormat          format,
    uint32_t          mip_levels,
    uint32_t          samples   = 1,
    VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
    VmaAllocationCreateFlags allocation_flags = 0);

void
vk_allocated_image_fini(
    const WenVkContext        *ctx,
    VmaAllocator               allocator,
    const WenVkAllocatedImage *allocated_image);

/**
 * @brief Creates a Vulkan image view for the given image.
 *
 * @param device        The Vulkan logical device.
 * @param image         The Vulkan image to create the view for.
 * @param image_type    The type of image view (e.g., VK_IMAGE_VIEW_TYPE_2D).
 * @param format        The format of the image (e.g.,
 * VK_FORMAT_R8G8B8A8_UNORM).
 * @param aspect_flags  Specifies which aspect(s) of the image are included
 * (e.g., VK_IMAGE_ASPECT_COLOR_BIT).
 * @param mip_levels    Number of mipmap levels to include in the view.
 * @return VkImageView  The created image view handle.
 */
[[nodiscard]] VkImageView
vk_image_view_init(
    VkDevice           device,
    VkImage            image,
    VkImageViewType    image_type,
    VkFormat           format,
    VkImageAspectFlags aspect_flags,
    uint32_t           mip_levels = 1);

/**
 * @brief Destroy a Vulkan image view.
 *
 * @param ctx           The Vulkan context (containing the logical device).
 * @param image_view    The Vulkan image view to destroy.
 */
void
vk_image_view_fini(const WenVkContext *ctx, VkImageView image_view);

/**
 * @brief Initializes a VkImageSubresourceRange structure.
 *
 * @param aspect_flags     Specifies the aspect(s) of the image.
 * @param layer_count      Number of array layers.
 * @param mip_level        Number of mip levels.
 * @param base_mip_level   The base mipmap level.
 * @return VkImageSubresourceRange structure.
 */
VkImageSubresourceRange
vk_image_subresource_range_init(
    VkImageAspectFlags aspect_flags,
    uint32_t           layer_count,
    uint32_t           mip_level,
    uint32_t           base_mip_level);

/**
 * @brief Blits from a source image to a destination image.
 *
 * @param cmd              The command buffer to record the blit into.
 * @param src              The source image.
 * @param dst              The destination image.
 * @param src_extent       The extent of the source image.
 * @param dst_extent       The extent of the destination image.
 * @param src_mip_levels   The source mip level (default is 0).
 * @param dst_mip_levels   The destination mip level (default is 0).
 * @param src_layer_count  Number of layers in the source image.
 * @param dst_layer_count  Number of layers in the destination image.
 */
void
vk_image_blit(
    VkCommandBuffer cmd,
    VkImage         src,
    VkImage         dst,
    VkExtent2D      src_extent,
    VkExtent2D      dst_extent,
    uint32_t        src_mip_levels = 0,
    uint32_t dst_mip_levels        = 0,
    uint32_t src_layer_count       = 1,
    uint32_t dst_layer_count       = 1);