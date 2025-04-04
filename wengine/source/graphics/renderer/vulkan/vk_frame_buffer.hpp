#pragma once

#include "vk_types.hpp"

void vk_frame_buffer_init(
    const WenVkContext *vk_ctx,
    VkDevice device,
    VmaAllocator allocator,
    uint32_t graphic_family_index,
    WenVkFrameBuffer *frame_buffer
);

void vk_frame_buffer_fini(
    const WenVkContext *vk_ctx,
    VkDevice device,
    WenVkFrameBuffer *frame_buffer
);

void vk_frame_buffer_reset_sync(
    const WenVkContext *vk_ctx,
    VkDevice device,
    WenVkFrameBuffer *frame_buffer
);