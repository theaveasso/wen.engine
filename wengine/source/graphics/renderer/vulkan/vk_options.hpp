#pragma once

#include <cstdint>
#include <vulkan/vulkan.h>

namespace vk_opts
{
    inline constexpr uint64_t         timeout_ns           = 1'000'000'000;
    inline constexpr VkPresentModeKHR desired_present_mode = VK_PRESENT_MODE_MAILBOX_KHR;

#if defined(WEN_DBG)
    inline constexpr bool validation_enabled = true;
#else
    inline constexpr bool validation_enabled = false;
#endif
}