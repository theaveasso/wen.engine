#ifndef RENDERER_VULKAN_VULKAN_UTILS_HPP_
#define RENDERER_VULKAN_VULKAN_UTILS_HPP_

#include <vulkan/vulkan.h>

/** @def To check Vulkan function calls in debug mode.
 *
 */
#if defined(WEN_DEBUG)
#define vk_check(expr)                           \
  do {                                           \
    VkResult err = expr;                         \
    if (err) {                                   \
      wen_error("Detected Vulkan error: ", err); \
      abort();                                   \
    };                                           \
  } while (0)
#endif

bool vk_result_is_success(VkResult result);

const char* vulkan_result_string(VkResult result, bool verbose = false);

#endif // RENDERER_VULKAN_VULKAN_UTILS_HPP_
