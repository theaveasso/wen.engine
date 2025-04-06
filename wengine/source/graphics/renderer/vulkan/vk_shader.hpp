#pragma once

#include <privates/wen_pch.hpp>

#include "vk_ext.hpp"

struct WenVkShader {
    VkShaderEXT           shader = VK_NULL_HANDLE;
    VkShaderStageFlagBits stage;
    VkShaderCreateInfoEXT shader_create_info;

    std::string path;
    std::string name = "unnamed shader";
};

struct WenVkShaderBuilder {
    std::vector<VkShaderCreateInfoEXT> create_infos;
    std::vector<std::vector<uint32_t>> spvs;
    std::vector<std::string>           names;
    std::vector<std::string>           paths;

    shaderc_compile_options_t shaderc_compile_options{};
    shaderc_compiler_t        compiler{};
};

struct WenVkShaderContext {
    std::vector<WenVkShader> vert_shaders;
    std::vector<WenVkShader> frag_shaders;

    WenVkShaderBuilder builder_shader;
};

enum struct WenVkShaderType { LINKED, UNLINKED };

void
shader_ctx_init(WenVkShaderContext *shader_ctx);

void
shader_ctx_fini(
    VkDevice                  device,
    const WenVkShaderContext *shader_ctx,
    const WenVkEXTContext    *ext_ctx);

void
shader_ctx_stage_shader(
    WenVkShaderContext              *shader_ctx,
    const std::filesystem::path     &filename,
    std::string_view                 name,
    std::span<VkDescriptorSetLayout> desc_set_layouts,
    std::span<VkPushConstantRange>   push_constant_ranges,
    VkShaderStageFlagBits            stage,
    VkShaderStageFlags               next_stage);

void
shader_ctx_commit_shader(
    VkDevice               device,
    WenVkShaderContext    *shader_ctx,
    const WenVkEXTContext *ext_ctx,
    WenVkShaderType        type);

void
shader_ctx_replace_shader(
    VkDevice               device,
    WenVkShaderContext    *shader_ctx,
    const WenVkEXTContext *ext_ctx);