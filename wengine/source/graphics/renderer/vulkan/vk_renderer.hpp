#pragma once

#include "graphics/renderer/wen_renderer_types.hpp"
#include "privates/wen_pch.hpp"

#include "vk_command_buffer.hpp"
#include "vk_frame_buffer.hpp"
#include "vk_image.hpp"
#include "vk_swapchain.hpp"

struct WenRenderArea {
    glm::vec2 bottom_left{0};
    glm::vec2 scissor_dimension{0};
};

struct WenVkRenderer {
    WenVkContext                       vk_ctx{};
    VkExtent2D                         image_extent{};
    WenRenderArea                      render_area;
    VkClearValue                       scene_clear_value{};
    VkRenderingAttachmentInfo          scene_image_attachment{};
    VkRenderingAttachmentInfo          scene_color_attachment{};
    VkRenderingAttachmentInfo          scene_depth_attachment{};
    VkRenderingInfo                    scene_rendering_info{};
    WenVkSwapchainContext              swapchain_context;
    WenVkAllocatedImage                depth_image;
    WenVkAllocatedImage                color_image;
    VmaAllocator                       allocator{};
    VkFence                            immediate_fence{};
    WenVkCommandContext                immediate_cmd_ctx{};
    WenVkCommandContext                compute_cmd_ctx{};
    uint64_t                           frame_number = 1;
    uint8_t                            current_frame_index{};
    std::array<WenVkAllocatedImage, 3> images{};
    std::array<WenVkFrameBuffer, 3>    frames{};
    static constexpr uint32_t          msaa_samples = 1;
};

bool
vk_renderer_init(
    WenRendererBackend *renderer,
    WenWindow          *window,
    const char         *app_name,
    int32_t             width,
    int32_t             height);
void
vk_renderer_fini(WenRendererBackend *renderer);
void
vk_renderer_resize(
    WenRendererBackend *renderer, uint32_t width, uint32_t height);
bool
vk_renderer_begin_frame(WenRendererBackend *renderer, float dt);
bool
vk_renderer_end_frame(WenRendererBackend *renderer, float dt);

VkRenderingInfo
vk_rendering_info_init(
    std::span<VkRenderingAttachmentInfo *> color_attachments,
    const VkRenderingAttachmentInfo       *depth_attachment,
    VkExtent2D                             extent);

VkRenderingAttachmentInfo
vk_color_attachment_info_init(
    VkImageView         view,
    const VkClearValue *clear,
    VkAttachmentLoadOp  load_op,
    VkAttachmentStoreOp store_op,
    VkImageView         resolve_image_view = nullptr);

VkRenderingAttachmentInfo
vk_depth_attachment_info_init(
    VkImageView view, VkAttachmentLoadOp load_op, VkAttachmentStoreOp store_op);