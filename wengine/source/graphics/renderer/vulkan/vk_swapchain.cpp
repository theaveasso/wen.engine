#include "vk_swapchain.hpp"

#include <stdexcept>
#include <set>
#include <algorithm>

#include "vk_options.hpp"
#include "vk_image.hpp"
#include "vk_utils.hpp"

[[nodiscard]] static WenVkSwapchainSupportDetail query_support_detail(const WenVkContext *ctx);

static void swapchain_init(WenVkSwapchainContext *swapchain_ctx, const WenVkContext *ctx);

static void swapchain_fini(WenVkSwapchainContext *swapchain_ctx, const WenVkContext *ctx);

void vk_swapchain_ctx_init(WenVkSwapchainContext *swapchain_ctx, const WenVkContext *ctx) {
    swapchain_ctx->present_mode   = VK_PRESENT_MODE_FIFO_KHR;
    swapchain_ctx->support_detail = query_support_detail(ctx);
    for (const auto &mode : swapchain_ctx->support_detail.present_modes)
    {
        if (mode == vk_opts::desired_present_mode)
        {
            swapchain_ctx->present_mode = mode;
        }
    }
    swapchain_init(swapchain_ctx, ctx);
}

void vk_swapchain_ctx_fini(WenVkSwapchainContext *swapchain_ctx, const WenVkContext *ctx) {
    swapchain_fini(swapchain_ctx, ctx);
}

void vk_swapchain_ctx_reset(WenVkSwapchainContext *swapchain_ctx, const WenVkContext *ctx) {
    swapchain_fini(swapchain_ctx, ctx);
    swapchain_init(swapchain_ctx, ctx);
}

static WenVkSwapchainSupportDetail query_support_detail(const WenVkContext *ctx) {
    WenVkSwapchainSupportDetail support_detail;
    vk_check(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        ctx->physical_device,
        ctx->surface,
        &support_detail.capabilities
    ));

    uint32_t surface_format_count = 0;
    vk_check(vkGetPhysicalDeviceSurfaceFormatsKHR(
        ctx->physical_device,
        ctx->surface,
        &surface_format_count,
        nullptr
    ));
    if (surface_format_count > 0)
    {
        support_detail.formats.resize(surface_format_count);
        vk_check(vkGetPhysicalDeviceSurfaceFormatsKHR(
            ctx->physical_device,
            ctx->surface,
            &surface_format_count,
            support_detail.formats.data()
        ));
    }

    uint32_t present_modes_count = 0;
    vk_check(vkGetPhysicalDeviceSurfacePresentModesKHR(
        ctx->physical_device,
        ctx->surface,
        &present_modes_count,
        nullptr
    ));

    if (present_modes_count > 0)
    {
        support_detail.present_modes.resize(present_modes_count);
        vk_check(vkGetPhysicalDeviceSurfacePresentModesKHR(
            ctx->physical_device,
            ctx->surface,
            &present_modes_count,
            support_detail.present_modes.data()));
    }
    return support_detail;
}

static void swapchain_init(
    WenVkSwapchainContext *swapchain_ctx,
    const WenVkContext *ctx
) {
    swapchain_ctx->support_detail = query_support_detail(ctx);
    swapchain_ctx->extent         = swapchain_ctx->support_detail.capabilities.currentExtent;

    VkSurfaceFormatKHR surface_format = swapchain_ctx->support_detail.formats[0];
    for (const auto    &format : swapchain_ctx->support_detail.formats)
    {
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            surface_format = format;
        }
    }
    swapchain_ctx->format = surface_format.format;

    uint32_t desired_image_count = 3;
    if (swapchain_ctx->support_detail.capabilities.minImageCount != 0)
    {
        desired_image_count = std::clamp(
            desired_image_count,
            swapchain_ctx->support_detail.capabilities.minImageCount,
            swapchain_ctx->support_detail.capabilities.maxImageCount
        );
    }

    VkSwapchainCreateInfoKHR swapchain_create_info = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    swapchain_create_info.minImageCount    = desired_image_count;
    swapchain_create_info.imageFormat      = surface_format.format;
    swapchain_create_info.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    swapchain_create_info.imageColorSpace  = surface_format.colorSpace;
    swapchain_create_info.surface          = ctx->surface;
    swapchain_create_info.presentMode      = swapchain_ctx->present_mode;
    swapchain_create_info.imageExtent      = swapchain_ctx->extent;
    swapchain_create_info.preTransform     = swapchain_ctx->support_detail.capabilities.currentTransform;
    swapchain_create_info.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.imageArrayLayers = 1;

    const std::set unique_queue_families
                       {ctx->queue_families.graphics,
                        ctx->queue_families.present,
                        ctx->queue_families.transfer,
                        ctx->queue_families.compute};

    std::vector<uint32_t> queue_family_indices;

    for (const auto &family_index : unique_queue_families)
    {
        queue_family_indices.push_back(family_index);
    }

    if (unique_queue_families.size() > 1)
    {
        // using swapchain sharing mode CONCURRENT
        queue_family_indices.resize(unique_queue_families.size());
        swapchain_create_info.queueFamilyIndexCount = queue_family_indices.size();
        swapchain_create_info.pQueueFamilyIndices   = queue_family_indices.data();
        swapchain_create_info.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
    }
    else
    {
        // using swapchain sharing mode EXCLUSIVE
        swapchain_create_info.queueFamilyIndexCount = 0;
        swapchain_create_info.pQueueFamilyIndices   = nullptr;
        swapchain_create_info.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
    }

    vk_check(vkCreateSwapchainKHR(
        ctx->logical_device,
        &swapchain_create_info,
        ctx->allocator,
        &swapchain_ctx->swapchain
    ));

    uint32_t actual_image_count;
    vk_check(vkGetSwapchainImagesKHR(
        ctx->logical_device,
        swapchain_ctx->swapchain,
        &actual_image_count,
        nullptr
    ));

    swapchain_ctx->images.resize(actual_image_count);

    vk_check(vkGetSwapchainImagesKHR(
        ctx->logical_device,
        swapchain_ctx->swapchain,
        &actual_image_count,
        swapchain_ctx->images.data()
    ));

    for (const auto &image : swapchain_ctx->images)
    {
        swapchain_ctx->image_views.push_back(
            vk_image_view_init(
                ctx->logical_device,
                image,
                VK_IMAGE_VIEW_TYPE_2D,
                swapchain_ctx->format,
                VK_IMAGE_ASPECT_COLOR_BIT
            ));
    }
}

static void swapchain_fini(WenVkSwapchainContext *swapchain_ctx, const WenVkContext *ctx) {
    vkDestroySwapchainKHR(ctx->logical_device, swapchain_ctx->swapchain, ctx->allocator);
    for (const auto &image_view : swapchain_ctx->image_views)
    {
        vk_image_view_fini(ctx, image_view);
    }
    swapchain_ctx->images.clear();
    swapchain_ctx->image_views.clear();
}
