#pragma once

#include <privates/wen_pch.hpp>

#include "vk_ext.hpp"

#define BUILTIN_OBJECT_SHADER "JJK"

struct WenVkShader
{
	std::string           name = "unnamed shader";
	std::string           path;
	VkShaderEXT           shader = VK_NULL_HANDLE;
	VkShaderStageFlagBits stage;
	VkShaderCreateInfoEXT shader_create_info;
};

struct WenVkShaderBuilder
{
	std::vector<VkShaderCreateInfoEXT> create_infos;
	std::vector<std::vector<uint32_t>> spvs;
	std::vector<std::string>           names;
	std::vector<std::string>           paths;
	shaderc_compiler_t                 compiler{};
	shaderc_compile_options_t          shaderc_compile_options{};
};

struct WenVkShaderContext
{
	std::vector<WenVkShader> vert_shaders;
	std::vector<WenVkShader> frag_shaders;
	WenVkShaderBuilder       builder;
};

enum struct WenVkShaderType
{
	LINKED,
	UNLINKED
};

void vk_shader_ctx_init(
    WenVkShaderContext *shader_ctx);

void vk_shader_ctx_fini(
    VkDevice                  device,
    const WenVkShaderContext *shader_ctx,
    const WenVkEXTContext    *ext_ctx);

void vk_shader_ctx_stage_shader(
    WenVkShaderContext              *shader_ctx,
    const std::filesystem::path     &filename,
    std::string_view                 name,
    std::span<VkDescriptorSetLayout> desc_set_layouts,
    std::span<VkPushConstantRange>   push_constant_ranges,
    VkShaderStageFlagBits            stage,
    VkShaderStageFlags               next_stage);

void vk_shader_ctx_commit_shader(
    VkDevice               device,
    WenVkShaderContext    *shader_ctx,
    const WenVkEXTContext *ext_ctx,
    WenVkShaderType        type);

void shader_ctx_replace_shader(
    VkDevice               device,
    WenVkShaderContext    *shader_ctx,
    const WenVkEXTContext *ext_ctx);