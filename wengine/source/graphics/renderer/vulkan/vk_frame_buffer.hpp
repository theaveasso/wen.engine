#pragma once

#include "vk_command_buffer.hpp"
#include "vk_mem_alloc.h"

struct WenVkFrameBuffer
{
	WenVkCommandContext command_context{};
	VkSemaphore         render_semaphore{};
	VkSemaphore         present_semaphore{};
	VkFence             render_fence{};
};

void vk_frame_buffer_init(
    const WenVkContext *vk_ctx,
    VkDevice            device,
    VmaAllocator        allocator,
    uint32_t            graphic_family_index,
    WenVkFrameBuffer   *frame_buffer);

void vk_frame_buffer_fini(
    const WenVkContext *vk_ctx,
    VkDevice            device,
    WenVkFrameBuffer   *frame_buffer);

void vk_frame_buffer_reset_sync(
    const WenVkContext *vk_ctx,
    VkDevice            device,
    WenVkFrameBuffer   *frame_buffer);