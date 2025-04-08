#pragma once

#include <vector>
#include <vulkan/vulkan.h>

struct WenVkContext;

struct WenVkSwapchainSupportDetail
{
	VkSurfaceCapabilitiesKHR        capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR>   present_modes;
};

struct WenVkSwapchainContext
{
	VkSwapchainKHR              swapchain;
	VkExtent2D                  extent;
	VkFormat                    format;
	VkPresentModeKHR            present_mode;
	std::vector<VkImage>        images;
	std::vector<VkImageView>    image_views;
	WenVkSwapchainSupportDetail support_detail;
};

void vk_swapchain_ctx_init(
    struct WenVkSwapchainContext *swapchain_ctx, const WenVkContext *ctx);

void vk_swapchain_ctx_fini(
    WenVkSwapchainContext *swapchain_ctx, const WenVkContext *ctx);

void vk_swapchain_ctx_reset(
    WenVkSwapchainContext *swapchain_ctx, const WenVkContext *ctx);
