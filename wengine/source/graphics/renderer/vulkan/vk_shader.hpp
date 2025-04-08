#pragma once

#include "privates/wen_pch.hpp"

#include "vk_context.hpp"
#include "vk_pipeline.hpp"
#include "vk_swapchain.hpp"

#define OBJECT_SHADER_STAGE_COUNT 2

struct WenVkShaderStage
{
	VkShaderModule                  shader_module = VK_NULL_HANDLE;
	VkPipelineShaderStageCreateInfo stage         = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
};

struct WenVkObjectShader
{
	WenVkPipelineInfo                                       pipeline_info;
	std::array<WenVkShaderStage, OBJECT_SHADER_STAGE_COUNT> shader_stages;
};

bool wen_vk_shader_object_init(VkDevice device, VkFormat format, WenVkObjectShader *object_shader);

void wen_vk_shader_object_fini(VkDevice device, WenVkObjectShader *object_shader);