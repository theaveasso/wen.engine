#include "vk_image.hpp"

#include "privates/wen_private_defines.hpp"

#include "vk_utils.hpp"
#include "vk_types.hpp"
#include <vk_mem_alloc.h>

WenVkAllocatedImage vk_allocated_image_init(
    VkDevice device,
    VmaAllocator allocator,
    VkImageUsageFlags image_usage,
    VkImageViewType view_type,
    VkExtent2D extent,
    VkFormat format,
    uint32_t mip_levels,
    uint32_t samples,
    VmaMemoryUsage memory_usage,
    VmaAllocationCreateFlags allocation_flags
) {
    VkExtent3D extent_3d{
        .width = extent.width,
        .height = extent.height,
        .depth = 1,
    };

    WenVkAllocatedImage image;
    image.extent3d   = extent_3d;
    image.format     = format;
    image.mip_levels = mip_levels;

    VkImageCreateInfo image_create_info = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    image_create_info.extent        = extent_3d;
    image_create_info.format        = format;
    image_create_info.usage         = image_usage;
    image_create_info.samples       = static_cast<VkSampleCountFlagBits>(samples);
    image_create_info.mipLevels     = mip_levels;
    image_create_info.imageType     = VK_IMAGE_TYPE_2D;
    image_create_info.arrayLayers   = view_type == VK_IMAGE_VIEW_TYPE_CUBE ? 6 : 1;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_create_info.flags         = view_type == VK_IMAGE_VIEW_TYPE_CUBE ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;

    VmaAllocationCreateInfo allocation_create_info{};
    allocation_create_info.usage = memory_usage;
    allocation_create_info.flags = allocation_flags;

    vk_check(vmaCreateImage(
        allocator,
        &image_create_info,
        &allocation_create_info,
        &image.image,
        &image.allocation,
        nullptr
    ));

    VkImageAspectFlags aspect_flags = VK_IMAGE_ASPECT_COLOR_BIT;
    if (format == VK_FORMAT_D32_SFLOAT)
    {
        aspect_flags = VK_IMAGE_ASPECT_DEPTH_BIT;
    }

    image.view = vk_image_view_init(
        device,
        image.image,
        view_type,
        format,
        aspect_flags,
        image.mip_levels
    );

    return image;
}

void vk_allocated_image_fini(
    const WenVkContext *ctx,
    VmaAllocator allocator,
    const WenVkAllocatedImage *allocated_image
) {
    vmaDestroyImage(allocator, allocated_image->image, allocated_image->allocation);
    vk_image_view_fini(ctx, allocated_image->view);
}

VkImageView vk_image_view_init(
    VkDevice device,
    VkImage image,
    VkImageViewType image_view_type,
    VkFormat format,
    VkImageAspectFlags aspect_flags,
    uint32_t mip_levels
) {
    VkImageViewCreateInfo image_view_create_info = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    image_view_create_info.format   = format;
    image_view_create_info.image    = image;
    image_view_create_info.viewType = image_view_type;

    uint32_t layer_count = image_view_type == VK_IMAGE_VIEW_TYPE_CUBE ? 6 : 1;

    auto subresource_range = vk_image_subresource_range_init(
        aspect_flags,
        layer_count,
        mip_levels,
        0
    );
    image_view_create_info.subresourceRange = subresource_range;

    VkImageView image_view;
    vk_check(vkCreateImageView(
        device,
        &image_view_create_info,
        nullptr,
        &image_view
    ));

    return image_view;
}

void vk_image_view_fini(
    const WenVkContext *ctx,
    VkImageView image_view
) {
    vkDestroyImageView(
        ctx->logical_device,
        image_view,
        ctx->allocator
    );
}

VkImageSubresourceRange vk_image_subresource_range_init(
    VkImageAspectFlags aspect_flags,
    uint32_t layer_count,
    uint32_t mip_level,
    uint32_t base_mip_level
) {
    VkImageSubresourceRange subresource_range;
    subresource_range.aspectMask     = aspect_flags;
    subresource_range.baseMipLevel   = base_mip_level;
    subresource_range.levelCount     = mip_level;
    subresource_range.baseArrayLayer = 0;
    subresource_range.layerCount     = layer_count;

    return subresource_range;
}

void
vk_image_blit(
    VkCommandBuffer cmd,
    VkImage src,
    VkImage dst,
    VkExtent2D src_extent,
    VkExtent2D dst_extent,
    uint32_t src_mip_levels,
    uint32_t dst_mip_levels,
    uint32_t src_layer_count,
    uint32_t dst_layer_count
) {
    VkImageBlit2 blit_region = {VK_STRUCTURE_TYPE_IMAGE_BLIT_2};
    blit_region.srcOffsets[0] = {0, 0, 0};
    blit_region.srcOffsets[1].x = WEN_CAST(int32_t, src_extent.width);
    blit_region.srcOffsets[1].y = WEN_CAST(int32_t, src_extent.height);
    blit_region.srcOffsets[1].z = 1;

    blit_region.dstOffsets[0] = {0, 0, 0};
    blit_region.dstOffsets[1].x = WEN_CAST(int32_t, dst_extent.width);
    blit_region.dstOffsets[1].y = WEN_CAST(int32_t, dst_extent.height);
    blit_region.dstOffsets[1].z = 1;

    blit_region.srcSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    blit_region.srcSubresource.baseArrayLayer = 0;
    blit_region.srcSubresource.layerCount     = src_layer_count;
    blit_region.srcSubresource.mipLevel       = src_mip_levels;

    blit_region.dstSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    blit_region.dstSubresource.baseArrayLayer = 0;
    blit_region.dstSubresource.layerCount     = dst_layer_count;
    blit_region.dstSubresource.mipLevel       = dst_mip_levels;

    VkBlitImageInfo2 blit_image_info = {VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2};
    blit_image_info.dstImage       = dst;
    blit_image_info.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    blit_image_info.srcImage       = src;
    blit_image_info.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    blit_image_info.filter         = VK_FILTER_LINEAR;
    blit_image_info.regionCount    = 1;
    blit_image_info.pRegions       = &blit_region;

    vkCmdBlitImage2(
        cmd,
        &blit_image_info
    );
}
