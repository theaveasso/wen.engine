#ifndef RENDERER_VULKAN_VULKAN_TYPES_INL_
#define RENDERER_VULKAN_VULKAN_TYPES_INL_

#include <vulkan/vulkan.h>

typedef struct wen_vulkan_context_t {
  VkInstance             instance;
  VkAllocationCallbacks* allocator;
} wen_vulkan_context_t;

#endif // RENDERER_VULKAN_VULKAN_TYPES_INL_
