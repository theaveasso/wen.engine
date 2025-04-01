#ifndef RENDERER_VULKAN_VULKAN_TYPES_INL_
#define RENDERER_VULKAN_VULKAN_TYPES_INL_

#include "wen/core/wen_logger.hpp"
#include "wen/wen.hpp"
#include <vulkan/vulkan.h>

#if defined(WEN_DEBUG)
#define vk_check(expr, message) \
  do {                          \
    if (expr != VK_SUCCESS) {   \
      wen_error(message);       \
      return;                   \
    };                          \
  } while (0)
#endif

typedef struct wen_vulkan_swapchain_support_info_t {
  VkSurfaceCapabilitiesKHR capabilities;
  uint32_t                 format_count;
  VkSurfaceFormatKHR*      formats;
  uint32_t                 present_mode_count;
  VkPresentModeKHR*        present_modes;
} wen_vulkan_swapchain_support_info_t;

typedef struct wen_vulkan_device_t {
  VkPhysicalDevice                    physical_device;
  VkDevice                            logical_device;
  wen_vulkan_swapchain_support_info_t swapchain_support_info;

  int32_t graphics_queue_index;
  int32_t present_queue_index;
  int32_t transfer_queue_index;

  VkPhysicalDeviceFeatures         features;
  VkPhysicalDeviceProperties       properties;
  VkPhysicalDeviceMemoryProperties memory;
} wen_vulkan_device_t;

typedef struct wen_vulkan_context_t {
  VkInstance             instance;
  VkAllocationCallbacks* allocator;
  VkSurfaceKHR           surface;
#if defined(WEN_DEBUG)
  VkDebugUtilsMessengerEXT debug_messenger;
#endif
  wen_vulkan_device_t devices;
} wen_vulkan_context_t;

#endif // RENDERER_VULKAN_VULKAN_TYPES_INL_
