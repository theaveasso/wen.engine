#include "wen/renderer/vulkan/vulkan_backend.hpp"

#include "wen/core/wen_memory.hpp"
#include "wen/renderer/vulkan/vulkan_utils.hpp"

void vulkan_framebuffer_init(
    wen_vulkan_context_t*    context,
    wen_vulkan_renderpass_t* renderpass,
    uint32_t width, uint32_t height,
    uint32_t                  attachment_count,
    VkImageView*              attachments,
    wen_vulkan_framebuffer_t* out_framebuffer) {

  out_framebuffer->attachments =
      (VkImageView*)wen_memalloc(sizeof(VkImageView) * attachment_count, MEMORY_TAG_RENDERER);

  for (uint32_t i = 0; i < attachment_count; ++i) {
    out_framebuffer->attachments[i] = attachments[i];
  }
  out_framebuffer->renderpass       = renderpass;
  out_framebuffer->attachment_count = attachment_count;

  VkFramebufferCreateInfo fb_create_info = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
  fb_create_info.renderPass              = out_framebuffer->renderpass->handle;
  fb_create_info.attachmentCount         = out_framebuffer->attachment_count;
  fb_create_info.pAttachments            = out_framebuffer->attachments;
  fb_create_info.width                   = width;
  fb_create_info.height                  = height;
  fb_create_info.layers                  = 1;

  vk_check(vkCreateFramebuffer(
      context->devices.logical_device,
      &fb_create_info,
      context->allocator,
      &out_framebuffer->handle));
}

void vulkan_framebuffer_fini(wen_vulkan_context_t* context, wen_vulkan_framebuffer_t* framebuffer) {
  vkDestroyFramebuffer(context->devices.logical_device, framebuffer->handle, context->allocator);
  if (framebuffer->attachments) {
    //    wen_memfree(&framebuffer->attachments, sizeof(VkImageView) * framebuffer->attachment_count, MEMORY_TAG_RENDERER);
    //    framebuffer->attachments = nullptr;
  }
  framebuffer->handle           = nullptr;
  framebuffer->attachment_count = 0;
  framebuffer->renderpass       = nullptr;
}
