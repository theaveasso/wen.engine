#include "vk_pipeline.hpp"

struct WenVertex3d
{
	glm::vec3 position;
};

VkPipelineLayout vk_pipeline_layout_init(
    VkDevice                         device,
    std::span<VkDescriptorSetLayout> desc_set_layouts,
    std::span<VkPushConstantRange>   push_constant_ranges,
    VkPipelineLayoutCreateFlags      flags)
{
	VkPipelineLayoutCreateInfo pipeline_layout_ci = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
	pipeline_layout_ci.flags                      = flags;
	pipeline_layout_ci.setLayoutCount             = desc_set_layouts.size();
	pipeline_layout_ci.pSetLayouts                = desc_set_layouts.data();
	pipeline_layout_ci.pushConstantRangeCount     = push_constant_ranges.size();
	pipeline_layout_ci.pPushConstantRanges        = push_constant_ranges.data();

	VkPipelineLayout pipeline_layout;
	vk_check(vkCreatePipelineLayout(
	    device,
	    &pipeline_layout_ci,
	    nullptr,
	    &pipeline_layout));

	return pipeline_layout;
}

WenVkPipelineInfo vk_pipeline_info_init(
    VkDevice                                              device,
    VkViewport                                            viewport,
    VkRect2D                                              scissor,
    const std::vector<VkVertexInputAttributeDescription> &attributes,
    const std::vector<VkDescriptorSetLayout>             &set_layouts,
    std::vector<VkPipelineShaderStageCreateInfo>         &t,
    bool                                                  is_wireframe,
    const WenVkPipelineBuilder                           *pipeline_builder)
{
	VkPipeline       pipeline{};
	VkPipelineLayout pipeline_layout{};

	VkPipelineViewportStateCreateInfo pl_viewport_sci               = {VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
	pl_viewport_sci.viewportCount                                   = 1;
	pl_viewport_sci.pViewports                                      = &viewport;
	pl_viewport_sci.scissorCount                                    = 1;
	pl_viewport_sci.pScissors                                       = &scissor;

	VkPipelineRasterizationStateCreateInfo pl_rasterization_sci     = {VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
	pl_rasterization_sci.depthClampEnable                           = VK_FALSE;
	pl_rasterization_sci.rasterizerDiscardEnable                    = VK_FALSE;
	pl_rasterization_sci.polygonMode                                = is_wireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
	pl_rasterization_sci.lineWidth                                  = 1.0f;
	pl_rasterization_sci.cullMode                                   = VK_CULL_MODE_BACK_BIT;
	pl_rasterization_sci.frontFace                                  = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	pl_rasterization_sci.depthBiasEnable                            = VK_FALSE;
	pl_rasterization_sci.depthBiasConstantFactor                    = 0.0f;
	pl_rasterization_sci.depthBiasClamp                             = 0.0f;
	pl_rasterization_sci.depthBiasSlopeFactor                       = 0.0f;

	VkPipelineMultisampleStateCreateInfo pl_msaa_sci                = {VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
	pl_msaa_sci.sampleShadingEnable                                 = VK_FALSE;
	pl_msaa_sci.rasterizationSamples                                = VK_SAMPLE_COUNT_1_BIT;
	pl_msaa_sci.minSampleShading                                    = 1.0f;
	pl_msaa_sci.pSampleMask                                         = nullptr;
	pl_msaa_sci.alphaToCoverageEnable                               = VK_FALSE;
	pl_msaa_sci.alphaToOneEnable                                    = VK_FALSE;

	VkPipelineDepthStencilStateCreateInfo pl_depth_stencil_sci      = {VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
	pl_depth_stencil_sci.depthTestEnable                            = VK_TRUE;
	pl_depth_stencil_sci.depthWriteEnable                           = VK_TRUE;
	pl_depth_stencil_sci.depthCompareOp                             = VK_COMPARE_OP_LESS;
	pl_depth_stencil_sci.depthBoundsTestEnable                      = VK_FALSE;
	pl_depth_stencil_sci.stencilTestEnable                          = VK_FALSE;

	VkPipelineColorBlendAttachmentState pl_color_blend_attachment_s = {VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
	pl_color_blend_attachment_s.blendEnable                         = VK_TRUE;
	pl_color_blend_attachment_s.srcColorBlendFactor                 = VK_BLEND_FACTOR_SRC_ALPHA;
	pl_color_blend_attachment_s.dstColorBlendFactor                 = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	pl_color_blend_attachment_s.colorBlendOp                        = VK_BLEND_OP_ADD;
	pl_color_blend_attachment_s.srcAlphaBlendFactor                 = VK_BLEND_FACTOR_SRC_ALPHA;
	pl_color_blend_attachment_s.dstAlphaBlendFactor                 = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	pl_color_blend_attachment_s.alphaBlendOp                        = VK_BLEND_OP_ADD;
	pl_color_blend_attachment_s.colorWriteMask                      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo pl_color_blend_sci          = {VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
	pl_color_blend_sci.logicOpEnable                                = VK_FALSE;
	pl_color_blend_sci.logicOp                                      = VK_LOGIC_OP_COPY;
	pl_color_blend_sci.attachmentCount                              = 1;
	pl_color_blend_sci.pAttachments                                 = &pl_color_blend_attachment_s;

	std::array dynamic_states{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_LINE_WIDTH};

	VkPipelineDynamicStateCreateInfo pl_dynamic_state_sci = {VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
	pl_dynamic_state_sci.dynamicStateCount                = dynamic_states.size();
	pl_dynamic_state_sci.pDynamicStates                   = dynamic_states.data();

	VkVertexInputBindingDescription input_binding_desc;
	input_binding_desc.binding                                = 0;
	input_binding_desc.stride                                 = sizeof(WenVertex3d);
	input_binding_desc.inputRate                              = VK_VERTEX_INPUT_RATE_VERTEX;

	VkPipelineVertexInputStateCreateInfo vert_input_sci       = {VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
	vert_input_sci.vertexBindingDescriptionCount              = 1;
	vert_input_sci.pVertexBindingDescriptions                 = &input_binding_desc;
	vert_input_sci.vertexAttributeDescriptionCount            = attributes.size();
	vert_input_sci.pVertexAttributeDescriptions               = attributes.data();

	VkPipelineInputAssemblyStateCreateInfo input_assembly_sci = {VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
	input_assembly_sci.topology                               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	input_assembly_sci.primitiveRestartEnable                 = VK_FALSE;

	VkPipelineLayoutCreateInfo pl_layout_ci                   = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
	pl_layout_ci.setLayoutCount                               = 0;
	pl_layout_ci.pSetLayouts                                  = nullptr;

	VkGraphicsPipelineCreateInfo graphics_pl_ci               = {VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
	graphics_pl_ci.stageCount                                 = pipeline_builder->shader_stags_ci.size();
	graphics_pl_ci.pStages                                    = pipeline_builder->shader_stags_ci.data();
	graphics_pl_ci.pVertexInputState                          = &vert_input_sci;
	graphics_pl_ci.pInputAssemblyState                        = &input_assembly_sci;

	vk_check(vkCreatePipelineLayout(
	    device,
	    &pl_layout_ci,
	    nullptr,
	    &pipeline_layout));

	VkResult result = vkCreateGraphicsPipelines(
	    device,
	    nullptr,
	    1,
	    &graphics_pl_ci,
	    nullptr,
	    &pipeline);
	if (result != VK_SUCCESS)
	{
		WEN_ERROR("failed creating graphics pipeline");
	}

	return {
	    .pipeline        = pipeline,
	    .pipeline_layout = pipeline_layout,
	};

	//	    VkPipelineColorBlendStateCreateInfo color_blend_state_ci = {
	//	        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
	//	color_blend_state_ci.logicOpEnable   = VK_FALSE;
	//	color_blend_state_ci.logicOp         = VK_LOGIC_OP_COPY;
	//	color_blend_state_ci.attachmentCount = 1;
	//	color_blend_state_ci.pAttachments =
	//	    &pipeline_builder->pl_color_blend_attachment_s;
	//
	//	VkPipelineLayout pipeline_layout;
	//	vk_check(vkCreatePipelineLayout(
	//	    device,
	//	    &pipeline_builder->pl_layout_ci,
	//	    nullptr,
	//	    &pipeline_layout));
	//
	//	VkGraphicsPipelineCreateInfo pipeline_ci = {
	//	    VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
	//	pipeline_ci.pNext             = &pipeline_builder->rendering_state_ci;
	//	pipeline_ci.stageCount        = pipeline_builder->shader_stags_ci.size();
	//	pipeline_ci.pStages           = pipeline_builder->shader_stags_ci.data();
	//	pipeline_ci.pVertexInputState = &vertex_input_ci;
	//	pipeline_ci.pInputAssemblyState =
	//	    &pipeline_builder->input_assembly_state_ci;
	//	pipeline_ci.pDynamicState       = &dynamic_state_ci;
	//	pipeline_ci.pViewportState      = &viewport_ci;
	//	pipeline_ci.pRasterizationState = &pipeline_builder->rasterization_state_ci;
	//	pipeline_ci.pDepthStencilState  = &pipeline_builder->depth_stencil_state_ci;
	//	pipeline_ci.pColorBlendState    = &color_blend_state_ci;
	//	pipeline_ci.layout              = pipeline_layout;
	//
	//	vk_check(vkCreateGraphicsPipelines(
	//	    device, nullptr, 1, &pipeline_ci, nullptr, &pipeline));
	//
	//	return {.pipeline = pipeline, .pipeline_layout = pipeline_layout};
}

void vk_pipeline_set_shaders(
    WenVkPipelineBuilder *pipeline_builder,
    VkShaderModule        vert_shader,
    VkShaderModule        frag_shader,
    std::string_view      entry_name)
{
	VkPipelineShaderStageCreateInfo vert_stage_ci = {
	    VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
	vert_stage_ci.stage                           = VK_SHADER_STAGE_VERTEX_BIT;
	vert_stage_ci.pName                           = entry_name.data();
	vert_stage_ci.module                          = vert_shader;

	VkPipelineShaderStageCreateInfo frag_stage_ci = {
	    VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
	vert_stage_ci.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
	vert_stage_ci.pName  = entry_name.data();
	vert_stage_ci.module = frag_shader;

	pipeline_builder->shader_stags_ci.clear();
	pipeline_builder->shader_stags_ci.push_back(vert_stage_ci);
	pipeline_builder->shader_stags_ci.push_back(frag_stage_ci);
}

void vk_pipeline_set_layout(
    WenVkPipelineBuilder            *pipeline_builder,
    std::span<VkDescriptorSetLayout> desc_set_layouts,
    std::span<VkPushConstantRange>   push_constant_ranges,
    VkPipelineLayoutCreateFlags      flags)
{
	pipeline_builder->pipeline_layout_ci = {
	    VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
	pipeline_builder->pipeline_layout_ci.setLayoutCount =
	    desc_set_layouts.size();
	pipeline_builder->pipeline_layout_ci.pSetLayouts = desc_set_layouts.data();
	pipeline_builder->pipeline_layout_ci.pushConstantRangeCount =
	    push_constant_ranges.size();
	pipeline_builder->pipeline_layout_ci.pPushConstantRanges =
	    push_constant_ranges.data();
}
