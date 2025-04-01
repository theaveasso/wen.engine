#ifndef RENDERER_VULKAN_VULKAN_PLATFORM_HPP_
#define RENDERER_VULKAN_VULKAN_PLATFORM_HPP_

struct wen_platform_state_t;
struct wen_vulkan_context_t;

/**
 * Appends the name of required extensions for this platform to
 * the name vec, which should be created and pass in;
 */
void platform_get_required_exts_names(const char*** required_extensions_);

bool platform_create_vulkan_surface(struct wen_vulkan_context_t* context_, struct wen_platform_state_t* state_);

void platform_destroy_vulkan_surface(struct wen_vulkan_context_t* context_);

#endif // RENDERER_VULKAN_VULKAN_PLATFORM_HPP_
