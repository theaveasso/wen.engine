#pragma once

#include "privates/wen_pch.hpp"

// Code snippets from
// https://github.com/KhronosGroup/Vulkan-Samples/blob/f98af0cf3a769ce9d15d50baf2caafe43b311ca3/framework/common/strings.h

std::string
vk_result_str(VkResult result);

#define vk_check(expr)                                                         \
    do {                                                                       \
        VkResult err = (expr);                                                 \
        if (err != VK_SUCCESS) {                                               \
            WEN_ERROR("Vulkan Result Error {}", vk_result_str(err));           \
            abort();                                                           \
        }                                                                      \
    } while (0)
