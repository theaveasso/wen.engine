#include "wen/renderer/vulkan/vulkan_backend.hpp"

#include "wen/renderer/vulkan/vulkan_utils.hpp"
#include "wen/core/wen_logger.hpp"

void vulkan_image_init(
    wen_vulkan_context_t* context,
    VkImageType           image_type,
    uint32_t              width,
    uint32_t              height,
    VkFormat              format,
    VkImageTiling         tiling,
    VkImageUsageFlags     usage,
    VkMemoryPropertyFlags memory_flags,
    bool                  create_view,
    VkImageAspectFlags    view_aspect_flags,
    wen_vulkan_image_t*   out_image) {
  out_image->width  = width;
  out_image->height = height;

  VkImageCreateInfo image_create_info = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
  image_create_info.imageType         = image_type;
  image_create_info.extent.width      = out_image->width;
  image_create_info.extent.height     = out_image->height;
  image_create_info.extent.depth      = 1;
  image_create_info.mipLevels         = 4;
  image_create_info.arrayLayers       = 1;
  image_create_info.format            = format;
  image_create_info.tiling            = tiling;
  image_create_info.initialLayout     = VK_IMAGE_LAYOUT_UNDEFINED;
  image_create_info.usage             = usage;
  image_create_info.samples           = VK_SAMPLE_COUNT_1_BIT;
  image_create_info.sharingMode       = VK_SHARING_MODE_EXCLUSIVE;

  vk_check(vkCreateImage(
      context->devices.logical_device,
      &image_create_info,
      context->allocator,
      &out_image->handle));
  VkMemoryRequirements memory_requirements;
  vkGetImageMemoryRequirements(context->devices.logical_device, out_image->handle, &memory_requirements);

  int32_t memory_type = context->find_memory_index(memory_requirements.memoryTypeBits, memory_flags);
  if (memory_type == -1) {
    wen_error("");
  }

  VkMemoryAllocateInfo memalloc_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
  memalloc_info.allocationSize       = memory_requirements.size;
  memalloc_info.memoryTypeIndex      = memory_type;
  vk_check(vkAllocateMemory(
      context->devices.logical_device,
      &memalloc_info,
      context->allocator,
      &out_image->memory));
  vk_check(vkBindImageMemory(
      context->devices.logical_device,
      out_image->handle,
      out_image->memory,
      0));

  if (create_view) {
    out_image->view = nullptr;
    vulkan_image_view_init(context, format, out_image->handle, &out_image->view, view_aspect_flags);
  }
}

void vulkan_image_fini(
    wen_vulkan_context_t* context,
    wen_vulkan_image_t*   image) {
  if (image->view) {
    vkDestroyImageView(
        context->devices.logical_device,
        image->view,
        context->allocator);
    image->view = nullptr;
  }
  if (image->memory) {
    vkFreeMemory(
        context->devices.logical_device,
        image->memory,
        context->allocator);
    image->memory = nullptr;
  }
  if (image->handle) {
    vkDestroyImage(
        context->devices.logical_device,
        image->handle,
        context->allocator);
    image->handle = nullptr;
  }
}

void vulkan_image_view_init(
    wen_vulkan_context_t* context,
    VkFormat              format,
    VkImage               image,
    VkImageView*          view,
    VkImageAspectFlags    aspect_flags) {
  VkImageViewCreateInfo image_view_create_info = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
  image_view_create_info.image                 = image;
  image_view_create_info.viewType              = VK_IMAGE_VIEW_TYPE_2D;
  image_view_create_info.format                = format;

  image_view_create_info.subresourceRange.aspectMask     = aspect_flags;
  image_view_create_info.subresourceRange.baseMipLevel   = 0;
  image_view_create_info.subresourceRange.levelCount     = 1;
  image_view_create_info.subresourceRange.baseArrayLayer = 0;
  image_view_create_info.subresourceRange.layerCount     = 1;

  vk_check(vkCreateImageView(
      context->devices.logical_device,
      &image_view_create_info,
      context->allocator,
      view));
}
