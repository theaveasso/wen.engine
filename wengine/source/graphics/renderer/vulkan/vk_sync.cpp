#include "vk_sync.hpp"

#include "vk_utils.hpp"
#include "vk_image.hpp"

VkSemaphoreSubmitInfo vk_semaphore_submit_info_ini(
    VkSemaphore semaphore,
    VkPipelineStageFlags2 stages,
    uint64_t timeline_value
) {
    VkSemaphoreSubmitInfo semaphore_submit_info = {VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO};
    semaphore_submit_info.value       = timeline_value;
    semaphore_submit_info.semaphore   = semaphore;
    semaphore_submit_info.stageMask   = stages;
    semaphore_submit_info.deviceIndex = 0;

    return semaphore_submit_info;
}

VkSemaphore vk_semaphore_init(
    const WenVkContext *vk_ctx,
    VkDevice device,
    VkSemaphoreType type,
    uint64_t initial_timeline_value
) {
    VkSemaphore semaphore;

    VkSemaphoreCreateInfo     semaphore_create_info      = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    VkSemaphoreTypeCreateInfo semaphore_type_create_info = {VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO};

    semaphore_type_create_info.semaphoreType = type;
    semaphore_type_create_info.initialValue  = type & VK_SEMAPHORE_TYPE_BINARY ? 0 : initial_timeline_value;

    semaphore_create_info.pNext = &semaphore_type_create_info;
    semaphore_create_info.flags = 0;

    vk_check(vkCreateSemaphore(
        device,
        &semaphore_create_info,
        vk_ctx->allocator,
        &semaphore
    ));

    return semaphore;
}

void vk_semaphore_fini(
    const WenVkContext *vk_ctx,
    VkDevice device,
    VkSemaphore semaphore
) {
    vkDestroySemaphore(
        device,
        semaphore,
        vk_ctx->allocator
    );
}

VkFence vk_fence_init(
    const WenVkContext *vk_ctx,
    VkDevice device,
    VkFenceCreateFlags flags
) {
    VkFence fence;

    VkFenceCreateInfo fence_create_info = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    fence_create_info.flags = flags;

    vk_check(vkCreateFence(
        device,
        &fence_create_info,
        vk_ctx->allocator,
        &fence
    ));

    return fence;
}

void vk_fence_fini(
    const WenVkContext *vk_ctx,
    VkDevice device,
    VkFence fence
) {
    vkDestroyFence(
        device,
        fence,
        vk_ctx->allocator
    );
}

void vk_image_memory_barrie_insert(
    VkCommandBuffer cmd_buf,
    VkImage image,
    VkImageLayout current_layout,
    VkImageLayout new_layout,
    uint32_t mip_levels,
    uint32_t base_mip_level,
    uint32_t layer_count,
    VkPipelineStageFlags2 src_stages,
    VkPipelineStageFlags2 dst_stages
) {
    VkImageMemoryBarrier2 image_memory_barrier = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2};
    image_memory_barrier.image         = image;
    image_memory_barrier.srcStageMask  = src_stages;
    image_memory_barrier.dstStageMask  = dst_stages;
    image_memory_barrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;
    image_memory_barrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;
    image_memory_barrier.oldLayout     = current_layout;
    image_memory_barrier.newLayout     = new_layout;

    VkDependencyInfo dependency_info = {VK_STRUCTURE_TYPE_DEPENDENCY_INFO};

    VkImageAspectFlags aspect_flags = VK_IMAGE_ASPECT_COLOR_BIT;
    if (image_memory_barrier.newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        aspect_flags = VK_IMAGE_ASPECT_DEPTH_BIT;
    }

    VkImageSubresourceRange subresource_range = vk_image_subresource_range_init(
        aspect_flags,
        layer_count,
        mip_levels,
        base_mip_level
    );
    image_memory_barrier.subresourceRange = subresource_range;

    dependency_info.imageMemoryBarrierCount = 1;
    dependency_info.pImageMemoryBarriers    = &image_memory_barrier;

    vkCmdPipelineBarrier2(
        cmd_buf,
        &dependency_info
    );
}

