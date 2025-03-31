#ifndef RENDERER_VULKAN_VULKAN_TYPES_INL_
#define RENDERER_VULKAN_VULKAN_TYPES_INL_

#include "wen/core/wen_logger.hpp"
#include "wen/wen.hpp"
#include <vulkan/vulkan.h>

#if defined(WEN_DEBUG)
#define vk_check(expr) wen_assert(expr == VK_SUCCESS, "Vulkan operation failed.");
#else
#define vk_check(expr)                       \
  {                                          \
    if (!(expr == VK_SUCCESS)) {             \
      wen_error("Vulkan operation failed."); \
      return false;                          \
    }                                        \
  }
#endif

typedef struct wen_vulkan_context_t {
  VkInstance             instance;
  VkAllocationCallbacks* allocator;
#if defined(WEN_DEBUG)
  VkDebugUtilsMessengerEXT debug_messenger;
#endif
} wen_vulkan_context_t;

#endif // RENDERER_VULKAN_VULKAN_TYPES_INL_
