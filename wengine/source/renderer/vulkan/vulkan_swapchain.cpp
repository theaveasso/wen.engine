#include "wen/renderer/vulkan/vulkan_backend.hpp"

#include "wen/renderer/vulkan/vulkan_utils.hpp"

#include "wen/core/wen_logger.hpp"
#include "wen/core/wen_memory.hpp"
#include "wen/wen.hpp"

void init(wen_vulkan_context_t* context, uint32_t width, uint32_t height, wen_vulkan_swapchain_t* out_swapchain);
void fini(wen_vulkan_context_t* context, wen_vulkan_swapchain_t* out_swapchain);

void vulkan_swapchain_init(
    wen_vulkan_context_t*   context,
    uint32_t                width,
    uint32_t                height,
    wen_vulkan_swapchain_t* out_swapchain) {
  init(context, width, height, out_swapchain);
}

void vulkan_swapchain_reinit(
    wen_vulkan_context_t*   context,
    uint32_t                width,
    uint32_t                height,
    wen_vulkan_swapchain_t* out_swapchain) {
  fini(context, out_swapchain);
  init(context, width, height, out_swapchain);
}

void vulkan_swapchain_fini(
    wen_vulkan_context_t*   context,
    wen_vulkan_swapchain_t* swapchain) {
  fini(context, swapchain);
}

bool vulkan_swapchain_acquire_next_image_index(
    wen_vulkan_context_t*   context,
    wen_vulkan_swapchain_t* swapchain,
    uint64_t                timeout_ns,
    VkSemaphore             image_validation_semaphore,
    VkFence                 fence,
    uint32_t*               out_image_index) {
  VkResult result = vkAcquireNextImageKHR(
      context->devices.logical_device,
      swapchain->handle,
      timeout_ns,
      image_validation_semaphore,
      fence,
      out_image_index);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    vulkan_swapchain_reinit(
        context,
        context->framebuffer_width,
        context->framebuffer_height,
        swapchain);
    return false;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    wen_fatal("failed vkAcquireNextImageKHR!");
    return false;
  }

  return true;
}

void vulkan_swapchain_present(
    wen_vulkan_context_t*   context,
    wen_vulkan_swapchain_t* swapchain,
    VkQueue                 graphics_q,
    VkQueue                 present_q,
    VkSemaphore             render_complete_semaphore,
    uint32_t                present_image_index) {
  VkPresentInfoKHR present_info   = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores    = &render_complete_semaphore;
  present_info.swapchainCount     = 1;
  present_info.pSwapchains        = &swapchain->handle;
  present_info.pImageIndices      = &present_image_index;
  present_info.pResults           = nullptr;

  VkResult result = vkQueuePresentKHR(present_q, &present_info);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
    vulkan_swapchain_reinit(
        context,
        context->framebuffer_width,
        context->framebuffer_height,
        swapchain);
  } else if (result != VK_SUCCESS) {
    wen_fatal("failed vkAcquireNextImageKHR!");
  }
}

/** Init Swapchain. */
void init(
    wen_vulkan_context_t*   context,
    uint32_t                width,
    uint32_t                height,
    wen_vulkan_swapchain_t* out_swapchain) {
  VkExtent2D swapchain_extent         = {width, height};
  out_swapchain->max_frames_in_flight = 2;

  /** Choose swap surface format. */
  bool found = false;
  for (uint32_t i = 0; i < context->devices.swapchain_support_info.format_count; ++i) {
    VkSurfaceFormatKHR format = context->devices.swapchain_support_info.formats[i];
    if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
      out_swapchain->image_format = format;
      found                       = true;
      break;
    }
  }
  if (!found) {
    out_swapchain->image_format = context->devices.swapchain_support_info.formats[0];
  }

  /** Choose swap present mode. */
  VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
  for (uint32_t i = 0; i < context->devices.swapchain_support_info.present_mode_count; ++i) {
    VkPresentModeKHR mode = context->devices.swapchain_support_info.present_modes[i];
    if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
      present_mode = mode;
      break;
    }
  }

  vulkan_device_query_swapchain_support(
      context->devices.physical_device,
      context->surface,
      &context->devices.swapchain_support_info);
  if (context->devices.swapchain_support_info.capabilities.currentExtent.width != UINT32_MAX) {
    swapchain_extent = context->devices.swapchain_support_info.capabilities.currentExtent;
  }

  /** Choose swap extent. */
  VkExtent2D min          = context->devices.swapchain_support_info.capabilities.minImageExtent;
  VkExtent2D max          = context->devices.swapchain_support_info.capabilities.maxImageExtent;
  swapchain_extent.width  = WEN_CLAMP(swapchain_extent.width, min.width, max.width);
  swapchain_extent.height = WEN_CLAMP(swapchain_extent.height, min.height, max.height);

  /** Set image count. */
  uint32_t image_count = context->devices.swapchain_support_info.capabilities.minImageCount + 1;
  if (context->devices.swapchain_support_info.capabilities.minImageCount > 0 && image_count > context->devices.swapchain_support_info.capabilities.maxImageCount) {
    image_count = context->devices.swapchain_support_info.capabilities.maxImageCount;
  }

  /** Create swapchain. */
  VkSwapchainCreateInfoKHR swapchain_create_info = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
  swapchain_create_info.surface                  = context->surface;

  swapchain_create_info.minImageCount    = image_count;
  swapchain_create_info.imageFormat      = out_swapchain->image_format.format;
  swapchain_create_info.imageColorSpace  = out_swapchain->image_format.colorSpace;
  swapchain_create_info.imageExtent      = swapchain_extent;
  swapchain_create_info.imageArrayLayers = 1;
  swapchain_create_info.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  /** Find queue families. */
  if (context->devices.graphics_queue_index == context->devices.present_queue_index) {
    uint32_t queue_family_indices[] = {
        (uint32_t)context->devices.graphics_queue_index,
        (uint32_t)context->devices.present_queue_index,
    };
    swapchain_create_info.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
    swapchain_create_info.queueFamilyIndexCount = 2;
    swapchain_create_info.pQueueFamilyIndices   = queue_family_indices;
  } else {
    swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

  swapchain_create_info.preTransform   = context->devices.swapchain_support_info.capabilities.currentTransform;
  swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchain_create_info.presentMode    = present_mode;
  swapchain_create_info.clipped        = VK_TRUE;

  swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;

  vk_check(vkCreateSwapchainKHR(
      context->devices.logical_device,
      &swapchain_create_info,
      context->allocator,
      &out_swapchain->handle));

  context->current_frame     = 0;
  out_swapchain->image_count = 0;

  vk_check(vkGetSwapchainImagesKHR(
      context->devices.logical_device,
      out_swapchain->handle,
      &out_swapchain->image_count,
      nullptr));
  if (!out_swapchain->images) {
    out_swapchain->images = (VkImage*)wen_memalloc(
        sizeof(VkImage) * out_swapchain->image_count,
        MEMORY_TAG_RENDERER);
  }
  if (!out_swapchain->views) {
    out_swapchain->views = (VkImageView*)wen_memalloc(
        sizeof(VkImageView) * out_swapchain->image_count,
        MEMORY_TAG_RENDERER);
  }
  vk_check(vkGetSwapchainImagesKHR(
      context->devices.logical_device,
      out_swapchain->handle,
      &out_swapchain->image_count,
      out_swapchain->images));

  for (uint32_t i = 0; i < out_swapchain->image_count; ++i) {
    VkImageViewCreateInfo view_create_info           = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    view_create_info.image                           = out_swapchain->images[i];
    view_create_info.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    view_create_info.format                          = out_swapchain->image_format.format;
    view_create_info.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    view_create_info.subresourceRange.baseMipLevel   = 0;
    view_create_info.subresourceRange.levelCount     = 1;
    view_create_info.subresourceRange.baseArrayLayer = 0;
    view_create_info.subresourceRange.layerCount     = 1;

    vulkan_image_view_init(
        context,
        out_swapchain->image_format.format,
        out_swapchain->images[i],
        &out_swapchain->views[i],
        VK_IMAGE_ASPECT_COLOR_BIT);
  }

  if (!vulkan_device_detect_depth_format(&context->devices)) {
    context->devices.depth_format = VK_FORMAT_UNDEFINED;
    wen_fatal("vulkan_device_detect_depth_format : depth format UNDEFINED");
  }

  /** Creating Depth attachment. */
  vulkan_image_init(
      context,
      VK_IMAGE_TYPE_2D,
      swapchain_extent.width,
      swapchain_extent.height,
      context->devices.depth_format,
      VK_IMAGE_TILING_OPTIMAL,
      VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
      true,
      VK_IMAGE_ASPECT_DEPTH_BIT,
      &out_swapchain->depth_attachment);
}

void fini(
    wen_vulkan_context_t*   context,
    wen_vulkan_swapchain_t* swapchain) {
  /** Destroying Depth attachment. */
  vulkan_image_fini(context, &swapchain->depth_attachment);

  /** Destroyint swapchain image views. */
  for (uint32_t i = 0; i < swapchain->image_count; ++i) {
    vkDestroyImageView(
        context->devices.logical_device,
        swapchain->views[i],
        context->allocator);
  }

  /** Destroying swapchain. */
  vkDestroySwapchainKHR(
      context->devices.logical_device,
      swapchain->handle,
      context->allocator);
}
