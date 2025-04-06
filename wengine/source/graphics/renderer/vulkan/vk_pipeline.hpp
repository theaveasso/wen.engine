#pragma once

#include "privates/wen_pch.hpp"
#include "vk_shader.hpp"

struct WenVkPipelineInfo {
    VkPipeline       pipeline;
    VkPipelineLayout pipeline_layout;
};

struct WenVkPipelineBuilder {
    std::vector<VkPipelineShaderStageCreateInfo> shader_stags_ci;
    VkPipelineInputAssemblyStateCreateInfo       input_assembly_state_ci;
    VkPipelineRasterizationStateCreateInfo       rasterization_state_ci;
    VkPipelineDepthStencilStateCreateInfo        depth_stencil_state_ci;
    VkPipelineColorBlendStateCreateInfo          color_blend_state_ci;
    VkPipelineLayoutCreateInfo                   layout_state_ci;
    VkPipelineRenderingCreateInfo                rendering_state_ci;
    VkFormat                                     color_attachment_format_ci;
};

VkPipelineLayout
vk_pipeline_layout_init(
    VkDevice                         device,
    std::span<VkDescriptorSetLayout> desc_set_layouts,
    std::span<VkPushConstantRange>   push_constant_ranges,
    VkPipelineLayoutCreateFlags      flags);

WenVkPipelineInfo
vk_pipeline_info_init(
    VkDevice device, const WenVkPipelineBuilder *pipeline_builder);

void
vk_pipeline_set_shaders(
    WenVkPipelineBuilder *pipeline_builder,
    VkShaderModule        vert_shader,
    VkShaderModule        frag_shader,
    std::string_view      entry_name = "main");

void
vk_pipeline_set_layout(
    WenVkPipelineBuilder            *pipeline_builder,
    std::span<VkDescriptorSetLayout> desc_set_layouts,
    std::span<VkPushConstantRange>   push_constant_ranges,
    VkPipelineLayoutCreateFlags      flags);
