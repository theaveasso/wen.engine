#include "vk_pipeline.hpp"
#include "vk_shader.hpp"

#include "graphics/renderer/vulkan/common/vk_common.hpp"
#include "vk_utils.hpp"

bool vk_pipeline_info_init(
    VkDevice           device,
    VkFormat           color_attachment_formats,
    WenVkObjectShader *object_shader)
{
	VkPipelineLayoutCreateInfo layout_create_info = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
	vk_check(vkCreatePipelineLayout(
	    device,
	    &layout_create_info,
	    nullptr,
	    &object_shader->pipeline_info.layout));

	VkVertexInputBindingDescription binding_desc{
	    .binding   = 0,
	    .stride    = sizeof(WenVertex),
	    .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
	};

	std::array<VkVertexInputAttributeDescription, 2> attr_desc  = {{
        {.location = 0, .binding = 0, .format = VK_FORMAT_R32G32B32_SFLOAT, .offset = offsetof(WenVertex, position)},
        {.location = 1, .binding = 0, .format = VK_FORMAT_R32G32B32_SFLOAT, .offset = offsetof(WenVertex, color)},
    }};

	VkPipelineVertexInputStateCreateInfo vertex_input_state     = {VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
	vertex_input_state.vertexBindingDescriptionCount            = 1;
	vertex_input_state.pVertexBindingDescriptions               = &binding_desc;
	vertex_input_state.vertexAttributeDescriptionCount          = WEN_CAST(uint32_t, attr_desc.size());
	vertex_input_state.pVertexAttributeDescriptions             = attr_desc.data();

	VkPipelineInputAssemblyStateCreateInfo input_assembly_state = {VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
	input_assembly_state.topology                               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	input_assembly_state.primitiveRestartEnable                 = VK_FALSE;

	VkPipelineRasterizationStateCreateInfo rasterization_state  = {VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
	rasterization_state.depthClampEnable                        = VK_FALSE;
	rasterization_state.rasterizerDiscardEnable                 = VK_FALSE;
	rasterization_state.polygonMode                             = VK_POLYGON_MODE_FILL;
	rasterization_state.lineWidth                               = 1.0f;
	rasterization_state.cullMode                                = VK_CULL_MODE_BACK_BIT;
	rasterization_state.frontFace                               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterization_state.depthBiasEnable                         = VK_FALSE;
	rasterization_state.depthBiasConstantFactor                 = 0.0f;
	rasterization_state.depthBiasClamp                          = 0.0f;
	rasterization_state.depthBiasSlopeFactor                    = 0.0f;

	VkPipelineColorBlendAttachmentState attachments             = {VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
	attachments.blendEnable                                     = VK_TRUE;
	attachments.srcColorBlendFactor                             = VK_BLEND_FACTOR_SRC_ALPHA;
	attachments.dstColorBlendFactor                             = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	attachments.colorBlendOp                                    = VK_BLEND_OP_ADD;
	attachments.srcAlphaBlendFactor                             = VK_BLEND_FACTOR_SRC_ALPHA;
	attachments.dstAlphaBlendFactor                             = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	attachments.alphaBlendOp                                    = VK_BLEND_OP_ADD;
	attachments.colorWriteMask                                  = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo color_blend_state       = {VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
	color_blend_state.logicOpEnable                             = VK_FALSE;
	color_blend_state.logicOp                                   = VK_LOGIC_OP_COPY;
	color_blend_state.attachmentCount                           = 1;
	color_blend_state.pAttachments                              = &attachments;

	VkPipelineViewportStateCreateInfo viewport_state            = {VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
	viewport_state.viewportCount                                = 1;
	viewport_state.scissorCount                                 = 1;

	VkPipelineDepthStencilStateCreateInfo depth_stencil_state   = {VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
	depth_stencil_state.depthTestEnable                         = VK_TRUE;
	depth_stencil_state.depthWriteEnable                        = VK_TRUE;
	depth_stencil_state.depthCompareOp                          = VK_COMPARE_OP_LESS;
	depth_stencil_state.depthBoundsTestEnable                   = VK_FALSE;
	depth_stencil_state.stencilTestEnable                       = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multi_sample_state     = {VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
	multi_sample_state.sampleShadingEnable                      = VK_FALSE;
	multi_sample_state.rasterizationSamples                     = VK_SAMPLE_COUNT_1_BIT;
	multi_sample_state.minSampleShading                         = 1.0f;
	multi_sample_state.pSampleMask                              = nullptr;
	multi_sample_state.alphaToCoverageEnable                    = VK_FALSE;
	multi_sample_state.alphaToOneEnable                         = VK_FALSE;

	std::vector<VkDynamicState> dynamic_states                  = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
        VK_DYNAMIC_STATE_CULL_MODE,
        VK_DYNAMIC_STATE_FRONT_FACE,
        VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY};

	VkPipelineDynamicStateCreateInfo dynamic_state = {VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
	dynamic_state.dynamicStateCount                = WEN_CAST(uint32_t, dynamic_states.size());
	dynamic_state.pDynamicStates                   = dynamic_states.data();

	std::vector<VkPipelineShaderStageCreateInfo> stages;
	stages.reserve(object_shader->shader_stages.size());
	for (const auto &stage : object_shader->shader_stages)
	{
		stages.emplace_back(stage.stage);
	}

	VkPipelineRenderingCreateInfo pipeline_rendering_info = {VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO};
	pipeline_rendering_info.colorAttachmentCount          = 1;
	pipeline_rendering_info.pColorAttachmentFormats       = &color_attachment_formats;

	VkGraphicsPipelineCreateInfo create_info              = {VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
	create_info.pNext                                     = &pipeline_rendering_info;
	create_info.stageCount                                = WEN_CAST(uint32_t, stages.size());
	create_info.pStages                                   = stages.data();
	create_info.pVertexInputState                         = &vertex_input_state;
	create_info.pInputAssemblyState                       = &input_assembly_state;
	create_info.pViewportState                            = &viewport_state;
	create_info.pRasterizationState                       = &rasterization_state;
	create_info.pMultisampleState                         = &multi_sample_state;
	create_info.pDepthStencilState                        = &depth_stencil_state;
	create_info.pColorBlendState                          = &color_blend_state;
	create_info.pDynamicState                             = &dynamic_state;
	create_info.layout                                    = object_shader->pipeline_info.layout;

	// creating graphics pipeline
	VkResult result                                       = vkCreateGraphicsPipelines(
        device,
        VK_NULL_HANDLE,
        1,
        &create_info,
        VK_NULL_HANDLE,
        &object_shader->pipeline_info.pipeline);
	if (result != VK_SUCCESS)
	{
		return false;
	}

	return true;
}
