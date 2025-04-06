#pragma once

#include "privates/wen_pch.hpp"

#include "vk_context.hpp"

struct WenVkCommandContext {
    VkCommandBuffer primary_buffer{};
    VkCommandPool   primary_pool{};
};

void
vk_command_buffer_init(
    const WenVkContext         *vk_ctx,
    VkDevice                    device,
    uint32_t                    queue_family_index,
    VkCommandPoolCreateFlags    flags,
    struct WenVkCommandContext *cmd_ctx);

void
vk_command_buffer_fini(
    const WenVkContext *vk_ctx, VkDevice device, WenVkCommandContext *cmd_ctx);

void
vk_command_ctx_begin_primary_buffer(
    const WenVkCommandContext *cmd_ctx, VkCommandBufferUsageFlags flags);

void
vk_command_ctx_submit_primary_buffer(
    const WenVkCommandContext   *cmd_ctx,
    VkQueue                      queue,
    VkFence                      fence,
    const VkSemaphoreSubmitInfo *wait_semaphore_info,
    const VkSemaphoreSubmitInfo *signal_semaphore_info);
