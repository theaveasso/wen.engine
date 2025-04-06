#pragma once

#include "privates/wen_pch.hpp"

bool
compile_shader_spv(
    shaderc_compiler_t        compiler,
    shaderc_compile_options_t compile_options,
    std::string_view          filename,
    VkShaderStageFlagBits     shader_stage,
    std::vector<uint32_t>    *out_spv);