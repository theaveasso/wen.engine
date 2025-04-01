#ifndef RENDERER_VULKAN_VULKAN_SWAPCHAIN_HPP_
#define RENDERER_VULKAN_VULKAN_SWAPCHAIN_HPP_

#include "wen/renderer/vulkan/vulkan_types.inl"

void vulkan_swapchain_init(wen_vulkan_context_t* context, uint32_t width, uint32_t height, wen_vulkan_swapchain_t* out_swapchain);
void vulkan_swapchain_reinit(wen_vulkan_context_t* context, uint32_t width, uint32_t height, wen_vulkan_swapchain_t* out_swapchain);
void vulkan_swapchain_fini(wen_vulkan_context_t* context, wen_vulkan_swapchain_t* swapchain);
bool vulkan_swapchain_acquire_next_image_index(wen_vulkan_context_t* context, wen_vulkan_swapchain_t* swapchain, uint64_t timeout_ns, VkSemaphore image_validation_semaphore, VkFence fence, uint32_t* out_image_index);
void vulkan_swapchain_present(wen_vulkan_context_t* context, wen_vulkan_swapchain_t* swapchain, VkQueue graphics_q, VkQueue present_q, VkSemaphore render_complete_semaphore, uint32_t present_image_index);

#endif // RENDERER_VULKAN_VULKAN_SWAPCHAIN_HPP_
