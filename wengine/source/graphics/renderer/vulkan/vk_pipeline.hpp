#pragma once

#include <vulkan/vulkan.h>

#include "privates/wen_pch.hpp"

struct WenVkPipelineInfo
{
	VkPipeline       pipeline = VK_NULL_HANDLE;
	VkPipelineLayout layout   = VK_NULL_HANDLE;
};

struct WenVkPipelineBuilder
{
	std::vector<VkPipelineShaderStageCreateInfo> shader_stags_ci;

	VkPipelineViewportStateCreateInfo      pl_viewport_sci;
	VkPipelineRasterizationStateCreateInfo pl_rasterization_sci;
	VkPipelineVertexInputStateCreateInfo   pl_vert_input_sci;
	VkPipelineMultisampleStateCreateInfo   pl_msaa_sci;
	VkPipelineDepthStencilStateCreateInfo  pl_depth_stencil_sci;
	VkPipelineColorBlendAttachmentState    pl_color_blend_attachment_s;
	VkPipelineInputAssemblyStateCreateInfo pl_input_assembly_sci;
	VkVertexInputBindingDescription        vert_input_binding_desc;
	VkPipelineLayoutCreateInfo             pl_layout_ci;
	VkPipelineRenderingCreateInfo          pl_rendering_sci;

	VkFormat color_attachment_format;
};

bool vk_pipeline_info_init(
    VkDevice                  device,
    VkFormat                  color_attachment_formats,
    struct WenVkObjectShader *object_shader);

VkPipelineLayout vk_pipeline_layout_init(
    VkDevice                         device,
    std::span<VkDescriptorSetLayout> desc_set_layouts,
    std::span<VkPushConstantRange>   push_constant_ranges,
    VkPipelineLayoutCreateFlags      flags);