#ifndef RENDERER_VULKAN_VULKAN_TYPES_INL_
#define RENDERER_VULKAN_VULKAN_TYPES_INL_

#include <vulkan/vulkan.h>

#include "wen/core/wen_logger.hpp"
#include "wen/wen.hpp"

typedef struct wen_vulkan_fence_t {
  bool    is_signaled;
  VkFence handle;
} wen_vulkan_fence_t;

/** Represents a Vulkan image, including its dimensions and memory. */
typedef struct wen_vulkan_image_t {
  uint32_t width;  /**< Image width in pixels. */
  uint32_t height; /**< Image height in pixels. */

  VkImage        handle; /**< Vulkan image handle. */
  VkImageView    view;   /**< Vulkan image view. */
  VkDeviceMemory memory; /**< Device memory backing the image. */
} wen_vulkan_image_t;

typedef enum wen_vulkan_renderpass_state_t {
  RENDERPASS_STATE_READY,
  RENDERPASS_STATE_RECORDING,
  RENDERPASS_STATE_IN_RENDER_PASS,
  RENDERPASS_STATE_RECORDING_ENDED,
  RENDERPASS_STATE_SUBMITTED,
  RENDERPASS_STATE_NOT_ALLOCATED,
} wen_vulkan_renderpass_state_t;

typedef struct wen_vulkan_renderpass_t {
  VkRenderPass handle;
  float        x, y, w, h;
  float        r, g, b, a;

  float    depth;
  uint32_t stencil;
} wen_vulkan_renderpass_t;

typedef enum wen_vulkan_command_buffer_state_t {
  COMMAND_BUFFER_STATE_READY,
  COMMAND_BUFFER_STATE_RECORDING,
  COMMAND_BUFFER_STATE_IN_RENDER_PASS,
  COMMAND_BUFFER_STATE_RECORDING_ENDED,
  COMMAND_BUFFER_STATE_SUBMITTED,
  COMMAND_BUFFER_STATE_NOT_ALLOCATED,
} wen_vulkan_command_buffer_state_t;

typedef struct wen_vulkan_command_buffer_t {
  VkCommandBuffer                   handle;
  wen_vulkan_command_buffer_state_t state;
} wen_vulkan_command_buffer_t;

typedef struct wen_vulkan_framebuffer_t {
  uint32_t                 attachment_count;
  VkImageView*             attachments;
  wen_vulkan_renderpass_t* renderpass;
  VkFramebuffer            handle;
} wen_vulkan_framebuffer_t;

/** Represents a Vulkan swapchain, managing multiple images for rendering. */
typedef struct wen_vulkan_swapchain_t {
  uint32_t image_count;          /** */
  uint8_t  max_frames_in_flight; /** */

  VkSwapchainKHR handle; /** */

  VkImage*           images;       /** */
  VkImageView*       views;        /** */
  VkSurfaceFormatKHR image_format; /** */

  wen_vulkan_image_t        depth_attachment; /** */
  wen_vulkan_framebuffer_t* frame_buffers;
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

  VkCommandPool graphics_command_pool;
} wen_vulkan_device_t;

/**
 *
 */
typedef struct wen_vulkan_context_t {
  uint32_t framebuffer_width;
  uint32_t framebuffer_height;

  uint64_t framebuffer_size_generation;
  uint64_t framebuffer_size_last_generation;

  uint32_t image_index;
  uint32_t current_frame;

  VkInstance             instance;
  VkAllocationCallbacks* allocator;
  VkSurfaceKHR           surface;
  wen_vulkan_device_t    devices;

  wen_vulkan_swapchain_t  swapchain;
  wen_vulkan_renderpass_t main_renderpass;

  wen_vulkan_command_buffer_t* graphics_command_buffers;

  bool recreating_swapchain;
  int32_t (*find_memory_index)(uint32_t type_filter, uint32_t property_flags);
#if defined(WEN_DEBUG)
  VkDebugUtilsMessengerEXT debug_messenger;
#endif

  /** Sync objects. */
  VkSemaphore*         image_available_semaphores;
  VkSemaphore*         queue_complete_semaphores;
  wen_vulkan_fence_t*  in_flight_fence;
  uint32_t             in_flight_fence_count;
  wen_vulkan_fence_t** images_in_flight;
} wen_vulkan_context_t;

#endif // RENDERER_VULKAN_VULKAN_TYPES_INL_
