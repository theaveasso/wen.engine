#include "vk_frame_buffer.hpp"

#include "vk_sync.hpp"
#include "vk_command_buffer.hpp"

void vk_frame_buffer_init(
    const WenVkContext *vk_ctx,
    VkDevice device,
    VmaAllocator allocator,
    uint32_t graphic_family_index,
    WenVkFrameBuffer *frame_buffer
) {
    frame_buffer->render_semaphore  = vk_semaphore_init(vk_ctx, device);
    frame_buffer->present_semaphore = vk_semaphore_init(vk_ctx, device);
    frame_buffer->render_fence      = vk_fence_init(vk_ctx, device, VK_FENCE_CREATE_SIGNALED_BIT);

    vk_command_buffer_init(
        vk_ctx,
        device,
        graphic_family_index,
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        &frame_buffer->command_context
    );
}

void vk_frame_buffer_fini(
    const WenVkContext *vk_ctx,
    VkDevice device,
    WenVkFrameBuffer *frame_buffer
) {
    vk_command_buffer_fini(
        vk_ctx,
        device,
        &frame_buffer->command_context
    );

    vk_semaphore_fini(vk_ctx, device, frame_buffer->render_semaphore);
    vk_semaphore_fini(vk_ctx, device, frame_buffer->present_semaphore);
    vk_fence_fini(vk_ctx, device, frame_buffer->render_fence);
}

void vk_frame_buffer_reset_sync(
    const WenVkContext *vk_ctx,
    VkDevice device,
    WenVkFrameBuffer *frame_buffer
) {
    vk_semaphore_fini(vk_ctx, device, frame_buffer->render_semaphore);
    vk_semaphore_fini(vk_ctx, device, frame_buffer->present_semaphore);
    vk_fence_fini(vk_ctx, device, frame_buffer->render_fence);

    frame_buffer->render_semaphore  = vk_semaphore_init(vk_ctx, device);
    frame_buffer->present_semaphore = vk_semaphore_init(vk_ctx, device);
    frame_buffer->render_fence      = vk_fence_init(vk_ctx, device, VK_FENCE_CREATE_SIGNALED_BIT);
}
