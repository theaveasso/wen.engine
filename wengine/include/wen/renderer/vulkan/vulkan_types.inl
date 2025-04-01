#ifndef RENDERER_VULKAN_VULKAN_TYPES_INL_
#define RENDERER_VULKAN_VULKAN_TYPES_INL_

#include <vulkan/vulkan.h>

#include "wen/core/wen_logger.hpp"
#include "wen/wen.hpp"

/** @def To check Vulkan function calls in debug mode.
 *
 */
#if defined(WEN_DEBUG)
#define vk_check(expr)                           \
  do {                                           \
    VkResult err = expr;                         \
    if (err) {                                   \
      wen_error("Detected Vulkan error: ", err); \
      abort();                                   \
    };                                           \
  } while (0)
#endif

/** Represents a Vulkan image, including its dimensions and memory. */
typedef struct wen_vulkan_image_t {
  uint32_t width;  /**< Image width in pixels. */
  uint32_t height; /**< Image height in pixels. */

  VkImage        handle; /**< Vulkan image handle. */
  VkImageView    view;   /**< Vulkan image view. */
  VkDeviceMemory memory; /**< Device memory backing the image. */
} wen_vulkan_image_t;

/** Represents a Vulkan swapchain, managing multiple images for rendering. */
typedef struct wen_vulkan_swapchain_t {
  uint32_t image_count;          /** */
  uint8_t  max_frames_in_flight; /** */

  VkSwapchainKHR handle; /** */

  VkImage*           images;       /** */
  VkImageView*       views;        /** */
  VkSurfaceFormatKHR image_format; /** */

  wen_vulkan_image_t depth_attachment; /** */
} wen_vulkan_swapchain_t;

/**
 *
 */
typedef struct wen_vulkan_swapchain_support_info_t {
  VkSurfaceCapabilitiesKHR capabilities;

  uint32_t            format_count;
  VkSurfaceFormatKHR* formats;

  uint32_t          present_mode_count;
  VkPresentModeKHR* present_modes;
} wen_vulkan_swapchain_support_info_t;

/**
 *
 */
typedef struct wen_vulkan_device_t {
  VkFormat depth_format;

  VkQueue graphics_queue;
  int32_t graphics_queue_index;

  VkQueue present_queue;
  int32_t present_queue_index;

  VkQueue transfer_queue;
  int32_t transfer_queue_index;

  VkPhysicalDeviceMemoryProperties    memory;
  VkDevice                            logical_device;
  VkPhysicalDevice                    physical_device;
  VkPhysicalDeviceFeatures            features;
  VkPhysicalDeviceProperties          properties;
  wen_vulkan_swapchain_support_info_t swapchain_support_info;
} wen_vulkan_device_t;

/**
 *
 */
typedef struct wen_vulkan_context_t {
  uint32_t framebuffer_width;
  uint32_t framebuffer_height;
  uint32_t image_index;
  uint32_t current_frame;

  VkInstance             instance;
  VkAllocationCallbacks* allocator;
  VkSurfaceKHR           surface;
  wen_vulkan_device_t    devices;

  wen_vulkan_swapchain_t swapchain;

  bool recreating_swapchain;
  int32_t (*find_memory_index)(uint32_t type_filter, uint32_t property_flags);
#if defined(WEN_DEBUG)
  VkDebugUtilsMessengerEXT debug_messenger;
#endif
} wen_vulkan_context_t;

#endif // RENDERER_VULKAN_VULKAN_TYPES_INL_
