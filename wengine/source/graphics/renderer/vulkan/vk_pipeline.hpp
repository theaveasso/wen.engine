#pragma once

#include "privates/wen_pch.hpp"
#include "vk_shader.hpp"

struct WenVkPipelineInfo
{
	VkPipeline       pipeline;
	VkPipelineLayout pipeline_layout;
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

VkPipelineLayout vk_pipeline_layout_init(
    VkDevice                         device,
    std::span<VkDescriptorSetLayout> desc_set_layouts,
    std::span<VkPushConstantRange>   push_constant_ranges,
    VkPipelineLayoutCreateFlags      flags);

WenVkPipelineInfo vk_pipeline_info_init(
    VkDevice                    device,
    const WenVkPipelineBuilder *pipeline_builder);

void vk_pipeline_info_fini(
    VkDevice           device,
    WenVkPipelineInfo *pipeline_info);

void vk_pipeline_set_shaders(
    WenVkPipelineBuilder *pipeline_builder,
    VkShaderModule        vert_shader,
    VkShaderModule        frag_shader,
    std::string_view      entry_name = "main");

void vk_pipeline_set_layout(
    WenVkPipelineBuilder            *pipeline_builder,
    std::span<VkDescriptorSetLayout> desc_set_layouts,
    std::span<VkPushConstantRange>   push_constant_ranges,
    VkPipelineLayoutCreateFlags      flags);