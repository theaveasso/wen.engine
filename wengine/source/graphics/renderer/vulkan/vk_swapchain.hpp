#pragma once

#include "vk_types.hpp"
#include <vector>

void vk_swapchain_ctx_init(WenVkSwapchainContext *swapchain_ctx, const WenVkContext *ctx);
void vk_swapchain_ctx_fini(WenVkSwapchainContext *swapchain_ctx, const WenVkContext *ctx);
void vk_swapchain_ctx_reset(WenVkSwapchainContext *swapchain_ctx, const WenVkContext *ctx);
