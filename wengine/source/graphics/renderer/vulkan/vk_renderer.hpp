#pragma once

#include <vulkan/vulkan.h>

#include "vk_types.hpp"
#include "graphics/renderer/wen_renderer_types.hpp"

bool vk_renderer_init(WenRendererBackend *renderer,
                      WenWindow *window,
                      const char *app_name,
                      int32_t width,
                      int32_t height);
void vk_renderer_fini(WenRendererBackend *renderer);
void vk_renderer_resize(WenRendererBackend *renderer, uint32_t width, uint32_t height);
bool vk_renderer_begin_frame(WenRendererBackend *renderer, float dt);
bool vk_renderer_end_frame(WenRendererBackend *renderer, float dt);

VkRenderingInfo vk_rendering_info_init(
    std::span<VkRenderingAttachmentInfo *> color_attachments,
    const VkRenderingAttachmentInfo *depth_attachment,
    VkExtent2D extent
);

VkRenderingAttachmentInfo vk_color_attachment_info_init(
    VkImageView view,
    const VkClearValue *clear,
    VkAttachmentLoadOp load_op,
    VkAttachmentStoreOp store_op,
    VkImageView resolve_image_view = nullptr
);

VkRenderingAttachmentInfo vk_depth_attachment_info_init(
    VkImageView view,
    VkAttachmentLoadOp load_op,
    VkAttachmentStoreOp store_op
);
