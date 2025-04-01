#include "wen/renderer/vulkan/vulkan_backend.hpp"

#include "wen/renderer/vulkan/vulkan_utils.hpp"

void vulkan_fence_init(
    wen_vulkan_context_t* context,
    bool create_signaled,
    wen_vulkan_fence_t* out_fence) {
  out_fence->is_signaled = create_signaled;
  VkFenceCreateInfo fence_create_info = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
  if (out_fence->is_signaled) {
    fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
  }

  vk_check(vkCreateFence(
      context->devices.logical_device,
      &fence_create_info,
      context->allocator,
      &out_fence->handle));
}

void vulkan_fence_fini(wen_vulkan_context_t* context, wen_vulkan_fence_t* fence) {
  vkWaitForFences(context->devices.logical_device, 1, &fence->handle, true, UINT32_MAX);
  if (fence->handle) {
    vkDestroyFence(
        context->devices.logical_device,
        fence->handle,
        context->allocator);
    fence->handle = nullptr;
  }
  fence->is_signaled = false;
}

void vulkan_fence_reset(wen_vulkan_context_t* context, wen_vulkan_fence_t* fence) {
  if (fence->is_signaled) {
    vk_check(vkResetFences(
        context->devices.logical_device,
        1,
        &fence->handle));
  }
}

bool vulkan_fence_wait(wen_vulkan_context_t* context, wen_vulkan_fence_t* fence, uint64_t timeout_ns) {
  if (!fence->is_signaled) {
    VkResult result =
        vkWaitForFences(context->devices.logical_device, 1, &fence->handle, true, timeout_ns);
    switch (result) {
    case VK_SUCCESS:
      fence->is_signaled = true;
      return true;
    case VK_TIMEOUT:
      wen_error("vulkan_fence_wait - timeout");
      break;
    case VK_ERROR_OUT_OF_HOST_MEMORY:
      wen_error("vulkan_fence_wait - out of host memory");
      break;
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
      wen_error("vulkan_fence_wait - out of device memory");
      break;
    default:
      break;
    }
  } else {
    return true;
  }

  return false;
}
