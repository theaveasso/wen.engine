#ifndef RENDERER_VULKAN_VULKAN_BACKEND_HPP_
#define RENDERER_VULKAN_VULKAN_BACKEND_HPP_

#include "wen/renderer/wen_renderer_backend.hpp"
#include "vulkan_types.inl"

bool vulkan_renderer_backend_initialize(wen_renderer_backend_t* backend_, const char* name, struct wen_platform_state_t* state);
void vulkan_renderer_backend_shutdown(wen_renderer_backend_t* backend_);
void vulkan_renderer_backend_resize(wen_renderer_backend_t* backend, uint16_t width, uint16_t height);

bool vulkan_renderer_backend_begin_frame(wen_renderer_backend_t* backend_, float deltatime_);
bool vulkan_renderer_backend_end_frame(wen_renderer_backend_t* backend_, float deltatime_);

/** Platform related functions. */
void platform_get_required_exts_names(const char*** required_extensions);
bool vulkan_surface_init(struct wen_vulkan_context_t* context, struct wen_platform_state_t* state);
void vulkan_surface_fini(struct wen_vulkan_context_t* context);

/** Swapchain related functions. */
void vulkan_swapchain_init(wen_vulkan_context_t* context, uint32_t width, uint32_t height, wen_vulkan_swapchain_t* out_swapchain);
void vulkan_swapchain_reinit(wen_vulkan_context_t* context, uint32_t width, uint32_t height, wen_vulkan_swapchain_t* out_swapchain);
void vulkan_swapchain_fini(wen_vulkan_context_t* context, wen_vulkan_swapchain_t* swapchain);
bool vulkan_swapchain_acquire_next_image_index(wen_vulkan_context_t* context, wen_vulkan_swapchain_t* swapchain, uint64_t timeout_ns, VkSemaphore image_validation_semaphore, VkFence fence, uint32_t* out_image_index);
void vulkan_swapchain_present(wen_vulkan_context_t* context, wen_vulkan_swapchain_t* swapchain, VkQueue graphics_q, VkQueue present_q, VkSemaphore render_complete_semaphore, uint32_t present_image_index);

/** Device related functions. */
bool vulkan_device_init(wen_vulkan_context_t* context_);
void vulkan_device_fini(wen_vulkan_context_t* context_);
void vulkan_logical_device_init(wen_vulkan_context_t* context);
void vulkan_logical_device_fini(wen_vulkan_context_t* context);
void vulkan_device_query_swapchain_support(VkPhysicalDevice device_, VkSurfaceKHR surface_, wen_vulkan_swapchain_support_info_t* out_swapchain_support_);
bool vulkan_device_detect_depth_format(wen_vulkan_device_t *device);

/** Image related functions. */
void vulkan_image_init(wen_vulkan_context_t* context, VkImageType image_type, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memory_flags, bool create_view, VkImageAspectFlags view_aspect_flags, wen_vulkan_image_t* out_image);
void vulkan_image_fini(wen_vulkan_context_t* context, wen_vulkan_image_t* image);
void vulkan_image_view_init(wen_vulkan_context_t* context, VkFormat format, VkImage image, VkImageView *view, VkImageAspectFlags aspect_flags);

/** Renderpass related functions. */
void vulkan_renderpass_init(wen_vulkan_context_t* context, float x, float y, float w, float h, float r, float g, float b, float a, float depth, uint32_t stencil, wen_vulkan_renderpass_t* renderpass);
void vulkan_renderpass_fini(wen_vulkan_context_t* context, wen_vulkan_renderpass_t* renderpass);
void vulkan_renderpass_begin(wen_vulkan_command_buffer_t* cmd_buf, wen_vulkan_renderpass_t* renderpass, VkFramebuffer frame_buffer);
void vulkan_renderpass_end(wen_vulkan_command_buffer_t* cmd_buf, wen_vulkan_renderpass_t* renderpass);

/** Command buffer related functions. */
void vulkan_command_buffer_allocate(wen_vulkan_context_t* context, VkCommandPool pool, bool is_primary, wen_vulkan_command_buffer_t* out_command_buffer);
void vulkan_command_buffer_free(wen_vulkan_context_t* context, VkCommandPool pool, wen_vulkan_command_buffer_t* command_buffer);
void vulkan_command_buffer_begin(wen_vulkan_command_buffer_t* command_buffer, bool is_single_use, bool is_renderpass_continue, bool is_simultaneous_use);
void vulkan_command_buffer_end(wen_vulkan_command_buffer_t* command_buffer);
void vulkan_command_buffer_update_submitted(wen_vulkan_command_buffer_t* command_buffer);
void vulkan_command_buffer_update_reset(wen_vulkan_command_buffer_t* command_buffer);
void vulkan_command_buffer_alloc_and_begin_single_use(wen_vulkan_context_t* context, VkCommandPool pool, wen_vulkan_command_buffer_t* out_command_buffer);
void vulkan_command_buffer_alloc_and_end_single_use(wen_vulkan_context_t* context, VkCommandPool pool, wen_vulkan_command_buffer_t* command_buffer, VkQueue queue);

/** Framebuffer related functions. */
void vulkan_framebuffer_init(wen_vulkan_context_t* context, wen_vulkan_renderpass_t* renderpass, uint32_t width, uint32_t height, uint32_t attachment_count, VkImageView* attachments, wen_vulkan_framebuffer_t* out_framebuffer);
void vulkan_framebuffer_fini(wen_vulkan_context_t* context, wen_vulkan_framebuffer_t* framebuffer);

/** Fence related functions. */
void vulkan_fence_init(wen_vulkan_context_t* context, bool create_signaled, wen_vulkan_fence_t* out_fence);
void vulkan_fence_fini(wen_vulkan_context_t* context, wen_vulkan_fence_t* fence);
void vulkan_fence_reset(wen_vulkan_context_t* context, wen_vulkan_fence_t* fence);
bool vulkan_fence_wait(wen_vulkan_context_t* context, wen_vulkan_fence_t* fence, uint64_t timeout_ns);

#endif // RENDERER_VULKAN_VULKAN_BACKEND_HPP_
