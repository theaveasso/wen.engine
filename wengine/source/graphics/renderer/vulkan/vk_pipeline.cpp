#include "vk_pipeline.hpp"

VkPipelineLayout
vk_pipeline_layout_init(
    VkDevice                         device,
    std::span<VkDescriptorSetLayout> desc_set_layouts,
    std::span<VkPushConstantRange>   push_constant_ranges,
    VkPipelineLayoutCreateFlags      flags) {
}

WenVkPipelineInfo
vk_pipeline_info_init(
    VkDevice device, const WenVkPipelineBuilder *pipeline_builder) {
}

void
vk_pipeline_set_shaders(
    WenVkPipelineBuilder *pipeline_builder,
    VkShaderModule        vert_shader,
    VkShaderModule        frag_shader,
    std::string_view      entry_name) {
}

void
vk_pipeline_set_layout(
    WenVkPipelineBuilder            *pipeline_builder,
    std::span<VkDescriptorSetLayout> desc_set_layouts,
    std::span<VkPushConstantRange>   push_constant_ranges,
    VkPipelineLayoutCreateFlags      flags) {
}
