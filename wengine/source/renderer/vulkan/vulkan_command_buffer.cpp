#include "wen/renderer/vulkan/vulkan_backend.hpp"

#include "wen/renderer/vulkan/vulkan_utils.hpp"

#include "wen/core/wen_memory.hpp"

void vulkan_command_buffer_allocate(
    wen_vulkan_context_t*        context,
    VkCommandPool                pool,
    bool                         is_primary,
    wen_vulkan_command_buffer_t* out_command_buffer) {
  wen_memzero(out_command_buffer, sizeof(wen_vulkan_command_buffer_t));

  VkCommandBufferAllocateInfo allocate_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
  allocate_info.commandPool                 = pool;
  allocate_info.level                       = is_primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
  allocate_info.commandBufferCount          = 1;
  allocate_info.pNext                       = nullptr;

  out_command_buffer->state = COMMAND_BUFFER_STATE_NOT_ALLOCATED;
  vk_check(vkAllocateCommandBuffers(
      context->devices.logical_device,
      &allocate_info,
      &out_command_buffer->handle));

  out_command_buffer->state = COMMAND_BUFFER_STATE_READY;
}

void vulkan_command_buffer_free(
    wen_vulkan_context_t*        context,
    VkCommandPool                pool,
    wen_vulkan_command_buffer_t* command_buffer) {
  vkFreeCommandBuffers(
      context->devices.logical_device,
      pool,
      1,
      &command_buffer->handle);

  command_buffer->handle = nullptr;
  command_buffer->state  = COMMAND_BUFFER_STATE_NOT_ALLOCATED;
}

void vulkan_command_buffer_begin(
    wen_vulkan_command_buffer_t* command_buffer,
    bool                         is_single_use,
    bool                         is_renderpass_continue,
    bool                         is_simultaneous_use) {
  VkCommandBufferBeginInfo begin_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
  begin_info.flags                    = 0;
  if (is_single_use) {
    begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  } else if (is_renderpass_continue) {
    begin_info.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
  } else if (is_simultaneous_use) {
    begin_info.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
  }

  vk_check(vkBeginCommandBuffer(
      command_buffer->handle,
      &begin_info));
  command_buffer->state = COMMAND_BUFFER_STATE_RECORDING;
}

void vulkan_command_buffer_end(
    wen_vulkan_command_buffer_t* command_buffer) {
  vk_check(vkEndCommandBuffer(command_buffer->handle));
  command_buffer->state = COMMAND_BUFFER_STATE_RECORDING_ENDED;
}

void vulkan_command_buffer_update_submitted(
    wen_vulkan_command_buffer_t* command_buffer) {
  command_buffer->state = COMMAND_BUFFER_STATE_SUBMITTED;
}

void vulkan_command_buffer_update_reset(
    wen_vulkan_command_buffer_t* command_buffer) {
  command_buffer->state = COMMAND_BUFFER_STATE_READY;
}

void vulkan_command_buffer_alloc_and_begin_single_use(
    wen_vulkan_context_t*        context,
    VkCommandPool                pool,
    wen_vulkan_command_buffer_t* out_command_buffer) {
  vulkan_command_buffer_allocate(context, pool, true, out_command_buffer);
  vulkan_command_buffer_begin(out_command_buffer, true, false, false);
}

void vulkan_command_buffer_alloc_and_end_single_use(
    wen_vulkan_context_t*        context,
    VkCommandPool                pool,
    wen_vulkan_command_buffer_t* command_buffer,
    VkQueue queue) {
  vulkan_command_buffer_end(command_buffer);
  VkSubmitInfo submit_info       = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers    = &command_buffer->handle;

  vk_check(vkQueueSubmit(
      queue,
      1,
      &submit_info,
      nullptr));
  vk_check(vkQueueWaitIdle(queue));

  vulkan_command_buffer_free(context, pool, command_buffer);
}
