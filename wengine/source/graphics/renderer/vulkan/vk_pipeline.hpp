#pragma once

#include <vulkan/vulkan.h>

#include "privates/wen_pch.hpp"

struct WenVkPipelineInfo
{
	VkPipeline       pipeline = VK_NULL_HANDLE;
	VkPipelineLayout layout   = VK_NULL_HANDLE;
};

bool vk_pipeline_info_init(
    VkDevice                  device,
    VkFormat                  color_attachment_formats,
    struct WenVkObjectShader *object_shader);

void vk_pipeline_info_fini(
    VkDevice           device,
    WenVkPipelineInfo *pipeline_info);
