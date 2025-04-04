#pragma once

#include <cstdlib>
#include "core/wen_logger.hpp"

#define vk_check(expr) \
do { \
    VkResult err = (expr); \
    if (err != VK_SUCCESS) { \
        WEN_ERROR("Vk Result Error: {}", #expr);             \
        abort(); \
    } \
} while(0)
