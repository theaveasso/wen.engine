#include "wen/renderer/vulkan/vulkan_backend.hpp"

#include "wen/renderer/vulkan/vulkan_utils.hpp"
#include "wen/core/wen_memory.hpp"

void vulkan_renderpass_init(
    wen_vulkan_context_t* context,
    float x, float y, float w, float h,
    float r, float g, float b, float a,
    float depth, uint32_t stencil,
    wen_vulkan_renderpass_t* out_renderpass) {
  out_renderpass->x       = x;
  out_renderpass->y       = y;
  out_renderpass->w       = w;
  out_renderpass->h       = h;
  out_renderpass->r       = r;
  out_renderpass->g       = g;
  out_renderpass->b       = b;
  out_renderpass->a       = a;
  out_renderpass->depth   = depth;
  out_renderpass->stencil = stencil;

  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;

  uint32_t                attachment_desc_count = 2;
  VkAttachmentDescription attachment_descriptions[attachment_desc_count];

  /** Color attachment */
  VkAttachmentDescription color_attachment;
  color_attachment.format         = context->swapchain.image_format.format;
  color_attachment.samples        = VK_SAMPLE_COUNT_1_BIT;
  color_attachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED; /**< Do not expect any particular layout before render-pass start.*/
  color_attachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  color_attachment.flags          = 0;

  VkAttachmentReference color_attachment_reference;
  color_attachment_reference.attachment = 0; /**< Attachment description array index. */
  color_attachment_reference.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  /** Depth attachment */
  VkAttachmentDescription depth_attachment;
  depth_attachment.format         = context->devices.depth_format;
  depth_attachment.samples        = VK_SAMPLE_COUNT_1_BIT;
  depth_attachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depth_attachment.storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED; /**< Do not expect any particular layout before render-pass start.*/
  depth_attachment.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  depth_attachment.flags          = 0;

  VkAttachmentReference depth_attachment_reference;
  depth_attachment_reference.attachment = 1; /**< Attachment description array index. */
  depth_attachment_reference.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  attachment_descriptions[0] = color_attachment;
  attachment_descriptions[1] = depth_attachment;

  subpass.colorAttachmentCount    = 1;
  subpass.pColorAttachments       = &color_attachment_reference;
  subpass.pDepthStencilAttachment = &depth_attachment_reference;
  subpass.inputAttachmentCount    = 0;
  subpass.pInputAttachments       = nullptr;
  subpass.pResolveAttachments     = nullptr;
  subpass.preserveAttachmentCount = 0;

  VkSubpassDependency dependency;
  dependency.srcSubpass      = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass      = 0;
  dependency.srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask   = 0;
  dependency.dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  dependency.dependencyFlags = 0;

  VkRenderPassCreateInfo render_pass_create_info = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
  render_pass_create_info.attachmentCount        = attachment_desc_count;
  render_pass_create_info.pAttachments           = attachment_descriptions;
  render_pass_create_info.subpassCount           = 1;
  render_pass_create_info.pSubpasses             = &subpass;
  render_pass_create_info.dependencyCount        = 1;
  render_pass_create_info.pDependencies          = &dependency;
  render_pass_create_info.pNext                  = nullptr;
  render_pass_create_info.flags                  = 0;

  vk_check(vkCreateRenderPass(
      context->devices.logical_device,
      &render_pass_create_info,
      context->allocator,
      &out_renderpass->handle));
}

void vulkan_renderpass_fini(wen_vulkan_context_t* context, wen_vulkan_renderpass_t* renderpass) {
  if (renderpass && renderpass->handle) {
    vkDestroyRenderPass(context->devices.logical_device, renderpass->handle, context->allocator);
    renderpass->handle = nullptr;
  }
}

void vulkan_renderpass_begin(wen_vulkan_command_buffer_t* cmd_buf, wen_vulkan_renderpass_t* renderpass, VkFramebuffer frame_buffer) {
  VkRenderPassBeginInfo render_pass_begin_info    = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
  render_pass_begin_info.renderPass               = renderpass->handle;
  render_pass_begin_info.framebuffer              = frame_buffer;
  render_pass_begin_info.renderArea.offset.x      = (int32_t)renderpass->x;
  render_pass_begin_info.renderArea.offset.y      = (int32_t)renderpass->x;
  render_pass_begin_info.renderArea.extent.width  = (int32_t)renderpass->w;
  render_pass_begin_info.renderArea.extent.height = (int32_t)renderpass->h;

  VkClearValue clear_value[2];
  wen_memzero(clear_value, sizeof(VkClearValue) * 2);
  clear_value[0].color.float32[0]     = renderpass->r;
  clear_value[0].color.float32[1]     = renderpass->g;
  clear_value[0].color.float32[2]     = renderpass->b;
  clear_value[0].color.float32[3]     = renderpass->a;
  clear_value[1].depthStencil.depth   = renderpass->depth;
  clear_value[1].depthStencil.stencil = renderpass->stencil;

  render_pass_begin_info.clearValueCount = 2;
  render_pass_begin_info.pClearValues    = clear_value;

  vkCmdBeginRenderPass(cmd_buf->handle, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
  cmd_buf->state = COMMAND_BUFFER_STATE_IN_RENDER_PASS;
}

void vulkan_renderpass_end(wen_vulkan_command_buffer_t* cmd_buf, wen_vulkan_renderpass_t* renderpass) {
  vkCmdEndRenderPass(cmd_buf->handle);
  cmd_buf->state = COMMAND_BUFFER_STATE_RECORDING;
}
