#pragma once

struct WenVkEXTContext {
    PFN_vkCreateShadersEXT     vkCreateShadersEXT{};
    PFN_vkDestroyShaderEXT     vkDestroyShaderEXT{};
    PFN_vkCmdBindShadersEXT    vkCmdBindShadersEXT{};
    PFN_vkCmdSetPolygonModeEXT vkCmdSetPolygonModeEXT{};
};

/** @brief Loads a Vulkan extension function into the provided context.
 *
 * @param device The Vulkan device handle to load extension function.
 * @param ctx The context object where the function pointer will be stored.
 * @param proc The function name to load (e.g., `vkCreateShadersEXT`).
 */
#define WEN_LOAD_VK_EXT_PROC(ctx, device, proc)                                \
    ctx->proc =                                                                \
        reinterpret_cast<PFN_##proc>(vkGetDeviceProcAddr(device, #proc));

/** @brief Initializes a Vulkan extension context by loading required Vulkan
 * extensions.
 *
 * @param device The Vulkan device handle to load extension function.
 * @param ext_ctx A pointer to the `WenVKEXTContext` structure.
 */
void
vk_ext_ctx_init(VkDevice device, WenVkEXTContext *ext_ctx);
