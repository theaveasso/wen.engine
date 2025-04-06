#pragma once

#include "core/wen_logger.hpp"
#include <cstdlib>

#define vk_check(expr)                                                         \
    do {                                                                       \
        VkResult err = (expr);                                                 \
        if (err != VK_SUCCESS) {                                               \
            WEN_ERROR("Vk Result Error");                                      \
            abort();                                                           \
        }                                                                      \
    } while (0)
