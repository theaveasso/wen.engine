#ifndef RENDERER_VULKAN_VULKAN_IMAGE_HPP_
#define RENDERER_VULKAN_VULKAN_IMAGE_HPP_

#include "vulkan_types.inl"

/** vulkan_image_init
 *
 */
void vulkan_image_init(wen_vulkan_context_t* context, VkImageType image_type, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memory_flags, bool create_view, VkImageAspectFlags view_aspect_flags, wen_vulkan_image_t* out_image);

/** vulkan_image_fini
 *
 */
void vulkan_image_fini(wen_vulkan_context_t* context, wen_vulkan_image_t* image);

void vulkan_image_view_init(wen_vulkan_context_t* context, VkFormat format, wen_vulkan_image_t* image, VkImageAspectFlags aspect_flags);

#endif // RENDERER_VULKAN_VULKAN_IMAGE_HPP_
