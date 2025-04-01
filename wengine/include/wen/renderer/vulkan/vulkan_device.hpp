#ifndef RENDERER_VULKAN_VULKAN_DEVICE_HPP_
#define RENDERER_VULKAN_VULKAN_DEVICE_HPP_

#include "wen/renderer/vulkan/vulkan_types.inl"

bool vulkan_device_create(wen_vulkan_context_t* context_);

void vulkan_device_destroy(wen_vulkan_context_t* context_);

void vulkan_device_query_swapchain_support(
    VkPhysicalDevice                     device_,
    VkSurfaceKHR                         surface_,
    wen_vulkan_swapchain_support_info_t* out_swapchain_support_);

#endif // RENDERER_VULKAN_VULKAN_DEVICE_HPP_
