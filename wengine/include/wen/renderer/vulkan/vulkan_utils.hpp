#ifndef RENDERER_VULKAN_VULKAN_UTILS_HPP_
#define RENDERER_VULKAN_VULKAN_UTILS_HPP_

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


#endif // RENDERER_VULKAN_VULKAN_UTILS_HPP_
