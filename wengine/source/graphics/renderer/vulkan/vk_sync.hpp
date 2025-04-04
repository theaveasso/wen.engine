#pragma once

#include "vk_types.hpp"

VkSemaphoreSubmitInfo vk_semaphore_submit_info_ini(
    VkSemaphore semaphore,
    VkPipelineStageFlags2 stages,
    uint64_t timeline_value = 0
);

VkSemaphore vk_semaphore_init(
    const WenVkContext *vk_ctx,
    VkDevice device,
    VkSemaphoreType = VK_SEMAPHORE_TYPE_BINARY,
    uint64_t initial_timeline_value = 0
);

void vk_semaphore_fini(
    const WenVkContext *vk_ctx,
    VkDevice device,
    VkSemaphore semaphore
);

VkFence vk_fence_init(
    const WenVkContext *vk_ctx,
    VkDevice device,
    VkFenceCreateFlags flags
);

void vk_fence_fini(
    const WenVkContext *vk_ctx,
    VkDevice device,
    VkFence fence
);

void vk_image_memory_barrie_insert(
    VkCommandBuffer cmd_buf,
    VkImage image,
    VkImageLayout current_layout,
    VkImageLayout new_layout,
    uint32_t mip_levels = 1,
    uint32_t base_mip_level = 0,
    uint32_t layer_count = 1,
    VkPipelineStageFlags2 src_stages = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
    VkPipelineStageFlags2 dst_stages = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT
);
