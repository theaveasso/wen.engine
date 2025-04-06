#include "vk_command_buffer.hpp"

void
vk_command_buffer_init(
    const WenVkContext      *vk_ctx,
    VkDevice                 device,
    uint32_t                 queue_family_index,
    VkCommandPoolCreateFlags flags,
    WenVkCommandContext     *cmd_ctx) {
    VkCommandPoolCreateInfo command_pool_create_info = {
        VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    command_pool_create_info.queueFamilyIndex = queue_family_index;
    command_pool_create_info.flags            = flags;

    vk_check(vkCreateCommandPool(
        device,
        &command_pool_create_info,
        vk_ctx->allocator,
        &cmd_ctx->primary_pool));

    VkCommandBufferAllocateInfo command_buffer_create_info = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    command_buffer_create_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    command_buffer_create_info.commandBufferCount = 1;
    command_buffer_create_info.commandPool        = cmd_ctx->primary_pool;

    vk_check(vkAllocateCommandBuffers(
        device, &command_buffer_create_info, &cmd_ctx->primary_buffer));
}

void
vk_command_buffer_fini(
    const WenVkContext *vk_ctx, VkDevice device, WenVkCommandContext *cmd_ctx) {
    vkDestroyCommandPool(device, cmd_ctx->primary_pool, vk_ctx->allocator);
}

void
vk_command_ctx_begin_primary_buffer(
    const WenVkCommandContext *cmd_ctx, VkCommandBufferUsageFlags flags) {
    VkCommandBufferBeginInfo command_buffer_begin_info = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    command_buffer_begin_info.flags = flags;

    vk_check(vkBeginCommandBuffer(
        cmd_ctx->primary_buffer, &command_buffer_begin_info));
}

void
vk_command_ctx_submit_primary_buffer(
    const WenVkCommandContext   *cmd_ctx,
    VkQueue                      queue,
    VkFence                      fence,
    const VkSemaphoreSubmitInfo *wait_semaphore_info,
    const VkSemaphoreSubmitInfo *signal_semaphore_info) {
    vk_check(vkEndCommandBuffer(cmd_ctx->primary_buffer));

    VkCommandBufferSubmitInfo command_buffer_submit_info = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO};
    command_buffer_submit_info.deviceMask    = 0;
    command_buffer_submit_info.commandBuffer = cmd_ctx->primary_buffer;

    VkSubmitInfo2 submit_info          = {VK_STRUCTURE_TYPE_SUBMIT_INFO_2};
    submit_info.flags                  = 0;
    submit_info.commandBufferInfoCount = 1;
    submit_info.pCommandBufferInfos    = &command_buffer_submit_info;
    submit_info.waitSemaphoreInfoCount = wait_semaphore_info != nullptr ? 1 : 0;
    submit_info.pWaitSemaphoreInfos    = wait_semaphore_info;
    submit_info.signalSemaphoreInfoCount =
        signal_semaphore_info != nullptr ? 1 : 0;
    submit_info.pSignalSemaphoreInfos = signal_semaphore_info;

    vk_check(vkQueueSubmit2(queue, 1, &submit_info, fence));
}
