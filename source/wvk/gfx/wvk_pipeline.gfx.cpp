#include "wvk_pipeline.gfx.hpp"

#include <backends/imgui_impl_vulkan.h>
#include <glm/gtx/string_cast.hpp>
#include <imgui.h>

#include "wvk_gfx.hpp"
#include "wvk_init.gfx.hpp"
#include "wvk_inst.gfx.hpp"
#include "wvk_utils.gfx.hpp"
#include "wvk_vulkan.gfx.hpp"

namespace wvk::gfx
{

CSMShadowMapPipeline::CSMShadowMapPipeline(
    Instance                                     &instance,
    const std::array<float, NUM_SHADOW_CASCADES> &percents,
    std::string_view                              name)
{
	auto device = instance.get_device();

	VkShaderModule vertModule = load_shader_module(
	    device,
	    "Builtin.CSM.vert.glsl",
	    "csm_shader_vert");

	VkShaderModule fragModule = load_shader_module(
	    device,
	    "Builtin.CSM.frag.glsl",
	    "csm_shader_frag");

	const std::array<VkDescriptorSetLayout, 1> descLayouts = {
	    instance.get_bindless_desc_set_layout()};

	const std::array<VkPushConstantRange, 1> bufferRanges = {
	    VkPushConstantRange{
	        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
	        .offset     = 0,
	        .size       = sizeof(PushConstants)}};

	_pipeline_layout = create_pipeline_layout(
	    device,
	    descLayouts,
	    bufferRanges,
	    "csm_pipeline_layout_" + std::string(name));

	_pipeline =
	    PipelineBuilder(_pipeline_layout)
	        .set_name("csm_pipeline_" + std::string(name))
	        .set_shaders(vertModule, fragModule)
	        .set_input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
	        .set_polygon_mode(VK_POLYGON_MODE_FILL)
	        .enable_culling()
	        .set_multisampling_none()
	        .disable_blending()
	        .set_depth_format(VK_FORMAT_D32_SFLOAT)
	        .enable_depth(true, VK_COMPARE_OP_GREATER_OR_EQUAL)
	        .build(device);

	vkDestroyShaderModule(device, vertModule, VK_NULL_HANDLE);
	vkDestroyShaderModule(device, fragModule, VK_NULL_HANDLE);

	const auto createInfo = CreateTextureInfo{
	    .format = VK_FORMAT_D32_SFLOAT,
	    .usages = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
	    .extent = {
	        .width  = WVK_CAST(uint32_t, _shadow_map_texture_size),
	        .height = WVK_CAST(uint32_t, _shadow_map_texture_size),
	        .depth  = 1},
	    .baseArrayLayer = NUM_SHADOW_CASCADES};

	_csm_shadow_map_id = instance.make_texture(
	    createInfo,
	    "csm_shadow_map_" + std::string(name));

	const auto &csmTexture = instance.get_texture(_csm_shadow_map_id);

	for (size_t i = 0; i < NUM_SHADOW_CASCADES; ++i)
	{
		const VkImageSubresourceRange subresourceRange =
		    init::image_subresource_range(
		        VK_IMAGE_ASPECT_DEPTH_BIT,
		        1,
		        1,
		        WVK_CAST(uint32_t, i));

		const VkImageViewCreateInfo imageViewInfo =
		    init::image_view_create_info(
		        csmTexture->get_format(),
		        csmTexture->handle(),
		        subresourceRange);

		vk_assert(vkCreateImageView(
		    device,
		    &imageViewInfo,
		    VK_NULL_HANDLE,
		    &_csm_shadow_map_views[i]));

		add_debug_w_label(
		    device,
		    _csm_shadow_map_views[i],
		    VK_OBJECT_TYPE_IMAGE_VIEW,
		    "csm_image_view_" + std::to_string(i) + "_" + std::string(name));
	}
}

void CSMShadowMapPipeline::cleanup(
    Instance &instance)
{
	auto device = instance.get_device();

	for (auto &iv : _csm_shadow_map_views)
	{
		vkDestroyImageView(device, iv, VK_NULL_HANDLE);
	}

	vkDestroyPipelineLayout(device, _pipeline_layout, VK_NULL_HANDLE);
	vkDestroyPipeline(device, _pipeline, VK_NULL_HANDLE);
}

void CSMShadowMapPipeline::draw(
    VkCommandBuffer                     cmd,
    Instance                           &instance,
    MeshCache                          &meshCache,
    const std::vector<MeshDrawCommand> &drawCommands,
    bool                                shadowEnabled)
{
	auto device = instance.get_device();

	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);
	instance.bind_bindless_desc_set(cmd, _pipeline_layout);

	const auto &csmShadowMapTexture = instance.get_texture(_csm_shadow_map_id);

	transition_image(
	    cmd,
	    csmShadowMapTexture->handle(),
	    VK_IMAGE_LAYOUT_UNDEFINED,
	    VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

	for (size_t i = 0; i < NUM_SHADOW_CASCADES; ++i)
	{
		const auto viewport = VkViewport{
		    .x        = 0,
		    .y        = 0,
		    .width    = WVK_CAST(float, _shadow_map_texture_size),
		    .height   = WVK_CAST(float, _shadow_map_texture_size),
		    .minDepth = 0.0f,
		    .maxDepth = 1.0f};
		vkCmdSetViewport(cmd, 0, 1, &viewport);

		const auto scissor = VkRect2D{
		    .offset = {},
		    .extent = {
		        WVK_CAST(uint32_t, _shadow_map_texture_size),
		        WVK_CAST(uint32_t, _shadow_map_texture_size)}};
		vkCmdSetScissor(cmd, 0, 1, &scissor);

		const auto renderingInfo = RenderingInfo{
		    .renderArea = {
		        .offset = {},
		        .extent = {
		            WVK_CAST(uint32_t, _shadow_map_texture_size),
		            WVK_CAST(uint32_t, _shadow_map_texture_size)}},
		    .depthImageView       = _csm_shadow_map_views[i],
		    .depthImageClearValue = 0.f};

		const auto renderInfo = create_render_info(renderingInfo);

		vkCmdBeginRendering(cmd, &renderInfo.info);

		for (const auto &drawCommand : drawCommands)
		{
			const auto &mesh = meshCache.get_gpu_mesh(drawCommand.meshId);
			vkCmdBindIndexBuffer(cmd, mesh.indexBuffer->handle(), 0, VK_INDEX_TYPE_UINT32);

			const auto pc = PushConstants{
			    .mvp          = _csm_light_space_t[i] * drawCommand.transform,
			    .vertexBuffer = mesh.vertexBuffer->get_device_address(device),
			    .materialId   = drawCommand.materialId,
			};

			vkCmdPushConstants(
			    cmd,
			    _pipeline_layout,
			    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			    0,
			    sizeof(PushConstants),
			    &pc);

			vkCmdDrawIndexed(
			    cmd,
			    mesh.numIndices,
			    1,
			    0,
			    0,
			    0);
		}

		vkCmdEndRendering(cmd);
	}

	transition_image(
	    cmd,
	    csmShadowMapTexture->handle(),
	    VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
	    VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL);
}

SpotLightShadowMapPipeline::SpotLightShadowMapPipeline(
    wvk::gfx::Instance &instance,
    std::string_view    name)
{
}

void SpotLightShadowMapPipeline::cleanup(
    Instance &instance)
{
}

PointLightShadowMapPipeline::PointLightShadowMapPipeline(
    Instance        &instance,
    float            pointLightMaxRange,
    std::string_view name) :
    _point_light_max_range(pointLightMaxRange)
{
	auto device = instance.get_device();

	VkShaderModule vertModule = load_shader_module(
	    device,
	    "Builtin.ShadowMap.vert.glsl",
	    "shadowmap_shader_vert");

	VkShaderModule fragModule = load_shader_module(
	    device,
	    "Builtin.ShadowMap.frag.glsl",
	    "shadowmap_shader_frag");

	const std::array<VkDescriptorSetLayout, 1> descLayouts = {
	    instance.get_bindless_desc_set_layout()};

	const std::array<VkPushConstantRange, 1> bufferRanges = {
	    VkPushConstantRange{
	        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
	        .offset     = 0,
	        .size       = sizeof(PushConstants)}};

	_pipeline_layout = create_pipeline_layout(
	    device,
	    descLayouts,
	    bufferRanges,
	    "shadowmap_pipeline_layout_" + std::string(name));

	_pipeline =
	    PipelineBuilder(_pipeline_layout)
	        .set_name("shadowmap_pipeline_" + std::string(name))
	        .set_shaders(vertModule, fragModule)
	        .set_input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
	        .set_polygon_mode(VK_POLYGON_MODE_FILL)
	        .enable_culling()
	        .set_multisampling_none()
	        .disable_blending()
	        .set_depth_format(VK_FORMAT_D32_SFLOAT)
	        .enable_depth(true, VK_COMPARE_OP_LESS)
	        .build(device);

	vkDestroyShaderModule(device, vertModule, VK_NULL_HANDLE);
	vkDestroyShaderModule(device, fragModule, VK_NULL_HANDLE);

	_shadow_map_image_views.resize(6 * MAX_POINT_LIGHTS);
	for (size_t i = 0; i < MAX_POINT_LIGHTS; ++i)
	{
		const auto textureInfo = CreateTextureInfo{
		    .format = VK_FORMAT_D32_SFLOAT,
		    .usages = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		    .flags  = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
		    .extent = VkExtent3D{
		        .width  = WVK_CAST(uint32_t, _shadow_map_texture_size),
		        .height = WVK_CAST(uint32_t, _shadow_map_texture_size),
		        .depth  = 1},
		    .baseArrayLayer = 6 * MAX_POINT_LIGHTS,
		    .isCubeMap      = true};

		_shadow_maps[i] = instance.make_texture(
		    textureInfo,
		    "point_light_shadowmap_" + std::to_string(i) + "_" + std::string(name));

		const auto &shadowMapTexture = instance.get_texture(_shadow_maps[i]);

		for (size_t j = 0; j < 6; ++j)
		{
			const VkImageSubresourceRange subresourceRange =
			    init::image_subresource_range(
			        VK_IMAGE_ASPECT_DEPTH_BIT,
			        1,
			        1,
			        WVK_CAST(uint32_t, j));

			const VkImageViewCreateInfo imageViewInfo =
			    init::image_view_create_info(
			        VK_FORMAT_D32_SFLOAT,
			        shadowMapTexture->handle(),
			        subresourceRange);

			vk_assert(vkCreateImageView(
			    device,
			    &imageViewInfo,
			    VK_NULL_HANDLE,
			    &_shadow_map_image_views[i + j * 6]));

			add_debug_w_label(
			    device,
			    _shadow_map_image_views[i + j * 6],
			    VK_OBJECT_TYPE_IMAGE_VIEW,
			    "point_light_shadow_map_view" + std::to_string(i + j * 6) + "_" + std::string(name));
		}
	}

	// init point light shadow map cameras
	_view_projection_buffer = instance.make_nbuffer(
	    sizeof(glm::mat4) * 6 * MAX_POINT_LIGHTS,
	    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
	    "point_light_vps_buffer_" + std::string(name));
}

void PointLightShadowMapPipeline::cleanup(
    Instance &instance)
{
	auto device = instance.get_device();

	_view_projection_buffer->cleanup();

	for (auto &iv : _shadow_map_image_views)
	{
		vkDestroyImageView(device, iv, VK_NULL_HANDLE);
	}

	vkDestroyPipelineLayout(device, _pipeline_layout, VK_NULL_HANDLE);
	vkDestroyPipeline(device, _pipeline, VK_NULL_HANDLE);
}

void PointLightShadowMapPipeline::begin_draw(
    VkCommandBuffer               cmd,
    const Instance               &instance,
    const std::vector<LightData> &lightData,
    const std::vector<size_t>    &pointLightingIndices)
{
	_light_to_shadow_map_id.clear();
	_current_shadow_map_index = 0;

	_view_projection_buffer->upload(
	    cmd,
	    instance.get_current_frame_index(),
	    (void *) _point_light_shadow_map_vps.data(),
	    sizeof(_point_light_shadow_map_vps),
	    0);

	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);
	instance.bind_bindless_desc_set(cmd, _pipeline_layout);
}

void PointLightShadowMapPipeline::end_draw(
    VkCommandBuffer,
    Instance &)
{
}

void PointLightShadowMapPipeline::draw(
    VkCommandBuffer           cmd,
    Instance                 &instance,
    MeshCache                &meshCache,
    LightId                   lightId,
    const shared_ptr<Buffer> &materialsBuffer,
    const glm::vec3 & /*lightPosition*/,
    const shared_ptr<Buffer> & /*lightsBuffer*/,
    const std::vector<MeshDrawCommand> &drawCommands,
    bool                                shadowsEnabled)
{
	if (_current_shadow_map_index >= MAX_POINT_LIGHTS)
	{
		WVK_WARN("too many point lights with shadows");
		return;
	}

	const auto device = instance.get_device();

	const auto &shadowMap = instance.get_texture(_shadow_maps[_current_shadow_map_index]);

	transition_image(
	    cmd,
	    shadowMap->handle(),
	    VK_IMAGE_LAYOUT_UNDEFINED,
	    VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

	for (size_t i = 0; i < 6; ++i)
	{
		const auto viewport = VkViewport{
		    .x        = 0,
		    .y        = 0,
		    .width    = WVK_CAST(float, _shadow_map_texture_size),
		    .height   = WVK_CAST(float, _shadow_map_texture_size),
		    .minDepth = 0.0f,
		    .maxDepth = 1.0f};
		vkCmdSetViewport(cmd, 0, 1, &viewport);

		const auto scissor = VkRect2D{
		    .offset = {},
		    .extent = {
		        WVK_CAST(uint32_t, _shadow_map_texture_size),
		        WVK_CAST(uint32_t, _shadow_map_texture_size)}};
		vkCmdSetScissor(cmd, 0, 1, &scissor);

		const auto renderingInfo = RenderingInfo{
		    .renderArea = {
		        .offset = {},
		        .extent = {
		            WVK_CAST(uint32_t, _shadow_map_texture_size),
		            WVK_CAST(uint32_t, _shadow_map_texture_size)}},
		    .depthImageView       = _shadow_map_image_views[i + _current_shadow_map_index * 6],
		    .depthImageClearValue = 1.f};

		const auto renderInfo = create_render_info(renderingInfo);

		vkCmdBeginRendering(cmd, &renderInfo.info);

		for (auto &drawCommand : drawCommands)
		{
			if (!shadowsEnabled)
			{
				continue;
			}

			const auto &mesh = meshCache.get_gpu_mesh(drawCommand.meshId);

			vkCmdBindIndexBuffer(
			    cmd,
			    mesh.indexBuffer->handle(),
			    0,
			    VK_INDEX_TYPE_UINT32);

			const auto pc = PushConstants{
			    .model           = drawCommand.transform,
			    .vertexBuffer    = mesh.vertexBuffer->get_device_address(device),
			    .lightsBuffer    = _view_projection_buffer->handle()->get_device_address(device),
			    .materialsBuffer = materialsBuffer->get_device_address(device),
			    .materialId      = drawCommand.materialId,
			    .lightId         = lightId,
			    .vpsBufferIndex  = WVK_CAST(uint32_t, i + +_current_shadow_map_index * 6),
			    .farPlane        = _point_light_max_range};

			vkCmdPushConstants(
			    cmd,
			    _pipeline_layout,
			    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			    0,
			    sizeof(PushConstants),
			    &pc);

			vkCmdDrawIndexed(cmd, mesh.numIndices, 1, 0, 0, 0);
		}

		vkCmdEndRendering(cmd);
	}

	_light_to_shadow_map_id[lightId] = _shadow_maps[_current_shadow_map_index];

	transition_image(
	    cmd,
	    shadowMap->handle(),
	    VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
	    VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL);

	++_current_shadow_map_index;
}

MeshPipeline::MeshPipeline(
    Instance             &instance,
    VkFormat              textureFormat,
    VkFormat              depthFormat,
    VkSampleCountFlagBits samples,
    std::string_view      name)
{
	auto device = instance.get_device();

	VkShaderModule vertModule = load_shader_module(
	    device,
	    "Builtin.Mesh.vert.glsl",
	    "mesh_shader_vert");

	VkShaderModule fragModule = load_shader_module(
	    device,
	    "Builtin.Mesh.frag.glsl",
	    "mesh_shader_frag");

	const std::array<VkDescriptorSetLayout, 1> descLayouts = {
	    instance.get_bindless_desc_set_layout()};

	const std::array<VkPushConstantRange, 1> bufferRanges = {
	    VkPushConstantRange{
	        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
	        .offset     = 0,
	        .size       = sizeof(PushConstants)}};

	_pipeline_layout = create_pipeline_layout(
	    device,
	    descLayouts,
	    bufferRanges,
	    "pipeline_layout" + std::string(name));

	_pipeline =
	    PipelineBuilder(_pipeline_layout)
	        .set_name(name)
	        .set_shaders(vertModule, fragModule)
	        .set_input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
	        .set_polygon_mode(VK_POLYGON_MODE_FILL)
	        .enable_culling()
	        .set_multisampling(samples)
	        .disable_blending()
	        .set_color_attachment_format(textureFormat)
	        .set_depth_format(depthFormat)
	        .enable_depth(true, VK_COMPARE_OP_GREATER_OR_EQUAL)
	        .build(device);

	vkDestroyShaderModule(device, vertModule, VK_NULL_HANDLE);
	vkDestroyShaderModule(device, fragModule, VK_NULL_HANDLE);
}

void MeshPipeline::cleanup(
    Instance &instance)
{
	auto device = instance.get_device();

	vkDestroyPipelineLayout(device, _pipeline_layout, VK_NULL_HANDLE);
	vkDestroyPipeline(device, _pipeline, VK_NULL_HANDLE);
}

void MeshPipeline::draw(
    VkCommandBuffer              cmd,
    VkExtent2D                   renderExtent,
    Instance                    &instance,
    const shared_ptr<MeshCache> &meshCache,
    const shared_ptr<MaterialCache> & /*materialCache*/,
    const shared_ptr<Buffer>           &sceneDataBuffer,
    const std::vector<MeshDrawCommand> &drawCommands,
    const std::vector<size_t>          &sortedDrawCommands)
{
	auto device = instance.get_device();

	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);
	instance.bind_bindless_desc_set(cmd, _pipeline_layout);

	const auto viewport = VkViewport{
	    .x        = 0,
	    .y        = 0,
	    .width    = WVK_CAST(float, renderExtent.width),
	    .height   = WVK_CAST(float, renderExtent.height),
	    .minDepth = 0.0f,
	    .maxDepth = 1.0f};
	vkCmdSetViewport(cmd, 0, 1, &viewport);

	const auto scissor = VkRect2D{
	    .offset = {},
	    .extent = renderExtent};
	vkCmdSetScissor(cmd, 0, 1, &scissor);

	for (const auto &dcIdx : sortedDrawCommands)
	{
		const auto &drawCommand = drawCommands[dcIdx];
		const auto &mesh        = meshCache->get_gpu_mesh(drawCommand.meshId);

		const auto pc = PushConstants{
		    .transform       = drawCommand.transform,
		    .sceneDataBuffer = sceneDataBuffer->get_device_address(device),
		    .vertexBuffer    = mesh.vertexBuffer->get_device_address(device),
		    .materialId      = drawCommand.materialId};

		vkCmdPushConstants(
		    cmd,
		    _pipeline_layout,
		    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
		    0,
		    sizeof(PushConstants),
		    &pc);

		vkCmdDrawIndexed(
		    cmd,
		    mesh.numIndices,
		    1,
		    0,
		    0,
		    0);
	}
}

// ---------------------------------------------------------------------------------------------
// gfx::UIOverlayPipeline
// ---------------------------------------------------------------------------------------------

UIOverlayPipeline::UIOverlayPipeline(
    wvk::gfx::Instance &instance,
    VkFormat            drawImageFormat,
    std::string_view    name)
{
	auto device = instance.get_device();

	_indexBuffer = instance.make_nbuffer(
	    sizeof(ImDrawIdx) * IMGUI_MAX_INDEX_COUNT,
	    VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
	    std::string(name) + "imgui_idx_buffer");

	_vertexBuffer = instance.make_nbuffer(
	    sizeof(ImDrawVert) * IMGUI_MAX_VERTEX_COUNT,
	    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
	    std::string(name) + "imgui_vert_buffer");

	ImGuiIO &io = ImGui::GetIO();

	unsigned char *pixels = WVK_CAST(uint8_t *, nullptr);
	int            width  = 0;
	int            height = 0;

	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

	io.Fonts->Flags |= ImFontAtlasFlags_NoPowerOfTwoHeight;

	_font_texture_id = instance.make_texture_w_pixels(
	    VK_FORMAT_R8G8B8A8_UNORM,
	    VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
	    VkExtent2D{WVK_CAST(uint32_t, width), WVK_CAST(uint32_t, height)},
	    pixels,
	    "imgui font texture");

	io.Fonts->SetTexID(WVK_CAST(ImTextureID, _font_texture_id));

	VkShaderModule vertShader = load_shader_module(
	    instance.get_device(),
	    "Builtin.ImGui.vert.glsl",
	    "imgui_shader_vert");

	VkShaderModule fragShader = load_shader_module(
	    instance.get_device(),
	    "Builtin.ImGui.frag.glsl",
	    "imgui_shader_frag");

	const std::array<VkDescriptorSetLayout, 1> descLayouts = {
	    instance.get_bindless_desc_set_layout()};

	const std::array<VkPushConstantRange, 1> bufferRanges = {
	    VkPushConstantRange{
	        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
	        .offset     = 0,
	        .size       = sizeof(PushConstants)}};

	_pipeline_layout = create_pipeline_layout(
	    device,
	    descLayouts,
	    bufferRanges,
	    "imgui_pipeline_layout_" + std::string(name));

	_pipeline =
	    PipelineBuilder(_pipeline_layout)
	        .set_name("imgui_pipeline_")
	        .set_shaders(vertShader, fragShader)
	        .set_input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
	        .set_polygon_mode(VK_POLYGON_MODE_FILL)
	        .disable_culling()
	        .set_multisampling_none()
	        .enable_blending(
	            VK_BLEND_OP_ADD,
	            VK_BLEND_FACTOR_ONE,
	            VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
	            VK_BLEND_FACTOR_ONE,
	            VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA)
	        .set_color_attachment_format(drawImageFormat)
	        .disable_depth_test()
	        .build(instance.get_device());

	vkDestroyShaderModule(instance.get_device(), vertShader, VK_NULL_HANDLE);
	vkDestroyShaderModule(instance.get_device(), fragShader, VK_NULL_HANDLE);
}

void UIOverlayPipeline::cleanup(
    VkDevice device)
{
	vkDestroyPipelineLayout(device, _pipeline_layout, VK_NULL_HANDLE);
	vkDestroyPipeline(device, _pipeline, VK_NULL_HANDLE);

	_indexBuffer->cleanup();
	_vertexBuffer->cleanup();
}

void UIOverlayPipeline::draw(
    VkCommandBuffer &cmd,
    Instance        &instance,
    VkImageView      imageView,
    VkExtent2D       extent)
{
	const ImDrawData *drawData = ImGui::GetDrawData();

	if (drawData->TotalVtxCount == 0)
	{
		return;
	}

	if (drawData->TotalIdxCount > IMGUI_MAX_INDEX_COUNT || drawData->TotalVtxCount > IMGUI_MAX_VERTEX_COUNT)
	{
		WVK_WARN("UIOverlay: too many vertices/indices to render (max indices {}, max vertices {}) buffer resize is not yet implemented.", IMGUI_MAX_INDEX_COUNT, IMGUI_MAX_VERTEX_COUNT);
		WVK_ASSERT_MSG(false, "implement ImGui buffer resize!");
		return;
	}

	_copy_buffers(cmd, instance);

	const RenderInfo renderInfo = create_render_info(
	    {.renderArea     = {{0}, extent},
	     .colorImageView = imageView});

	vkCmdBeginRendering(cmd, &renderInfo.info);

	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);
	instance.bind_bindless_desc_set(cmd, _pipeline_layout);

	float targetWidth  = WVK_CAST(float, extent.width);
	float targetHeight = WVK_CAST(float, extent.height);

	VkViewport viewport = {};
	viewport.x          = 0;
	viewport.y          = 0;
	viewport.width      = targetWidth;
	viewport.height     = targetHeight;
	viewport.minDepth   = 0.0f;
	viewport.maxDepth   = 1.0f;

	vkCmdSetViewport(cmd, 0, 1, &viewport);

	const ImVec2 clipOffset = drawData->DisplayPos;
	const ImVec2 clipScale  = drawData->FramebufferScale;

	size_t globalIdxOffset = 0;
	size_t globalVtxOffset = 0;

	vkCmdBindIndexBuffer(
	    cmd,
	    _indexBuffer->handle()->handle(),
	    0,
	    VK_INDEX_TYPE_UINT16);

	for (int cmdListId = 0; cmdListId < drawData->CmdListsCount; ++cmdListId)
	{
		const ImDrawList &cmdList = *drawData->CmdLists[cmdListId];
		for (int cmdId = 0; cmdId < cmdList.CmdBuffer.Size; ++cmdId)
		{
			const auto &imCmd = cmdList.CmdBuffer[cmdId];
			if (imCmd.UserCallback)
			{
				if (imCmd.UserCallback != ImDrawCallback_ResetRenderState)
				{
					imCmd.UserCallback(&cmdList, &imCmd);
					continue;
				}
				WVK_ASSERT_MSG(false, "ImDrawCallback_ResetRenderState is not supported!");
			}

			if (imCmd.ElemCount == 0)
			{
				continue;
			}

			// project scissor/clipping rectangles into framebuffer space.
			ImVec2 clipMin((imCmd.ClipRect.x - clipOffset.x) * clipScale.x, (imCmd.ClipRect.y - clipOffset.y) * clipScale.y);
			ImVec2 clipMax((imCmd.ClipRect.z - clipOffset.x) * clipScale.x, (imCmd.ClipRect.w - clipOffset.y) * clipScale.y);

			// clamp to viewport as vkCmdSetScissor() won't accept values that are off bounds.
			clipMin.x = std::clamp(clipMin.x, 0.0f, targetWidth);
			clipMax.x = std::clamp(clipMax.x, 0.0f, targetWidth);
			clipMin.y = std::clamp(clipMin.y, 0.0f, targetHeight);
			clipMax.y = std::clamp(clipMax.y, 0.0f, targetHeight);
			if (clipMax.x <= clipMin.x || clipMax.y <= clipMin.y)
			{
				continue;
			}

			// apply scissor/clipping rectangle.
			VkRect2D scissor = {};
			scissor.offset   = {WVK_CAST(int32_t, clipMin.x), WVK_CAST(int32_t, clipMin.y)};
			scissor.extent   = {WVK_CAST(uint32_t, clipMax.x - clipMin.x), WVK_CAST(uint32_t, clipMax.y - clipMin.y)};
			vkCmdSetScissor(cmd, 0, 1, &scissor);

			// Bind descriptor set with font or user texture.
			TextureId textureId = instance.get_white_texture_id();
			if (imCmd.TextureId != ImTextureID())
			{
				textureId = to_bindless_texture_id(imCmd.TextureId);
				WVK_ASSERT_MSG(textureId != NULL_TEXTURE_ID, "");
			}

			bool                      textureIsSRGB = true;
			const shared_ptr<Texture> texture       = instance.get_texture(textureId);
			if (texture->get_format() == VK_FORMAT_R8G8B8A8_SRGB || texture->get_format() == VK_FORMAT_R16G16B16A16_SFLOAT)
			{
				textureIsSRGB = false;
			}

			const glm::vec2 scale     = {2.0f / drawData->DisplaySize.x, 2.0f / drawData->DisplaySize.y};
			const glm::vec2 translate = {-1.0f - drawData->DisplayPos.x * scale.x, -1.0f - drawData->DisplayPos.y * scale.y};

			const PushConstants pcs = {
			    .vertexBuffer  = _vertexBuffer->handle()->get_device_address(instance.get_device()),
			    .textureId     = WVK_CAST(uint32_t, textureId),
			    .textureIsSRGB = textureIsSRGB,
			    .translate     = translate,
			    .scale         = scale};

			vkCmdPushConstants(
			    cmd,
			    _pipeline_layout,
			    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			    0,
			    sizeof(PushConstants),
			    &pcs);

			vkCmdDrawIndexed(
			    cmd,
			    imCmd.ElemCount,
			    1,
			    imCmd.IdxOffset + WVK_CAST(uint32_t, globalIdxOffset),
			    WVK_CAST(int32_t, imCmd.VtxOffset + WVK_CAST(uint32_t, globalVtxOffset)),
			    0);
		}

		globalIdxOffset += cmdList.IdxBuffer.Size;
		globalVtxOffset += cmdList.VtxBuffer.Size;
	}

	vkCmdEndRendering(cmd);
}

void UIOverlayPipeline::_copy_buffers(
    VkCommandBuffer cmd,
    Instance       &instance)
{
	const ImDrawData *drawData = ImGui::GetDrawData();
	if (!drawData || drawData->CmdListsCount == 0)
	{
		return;
	}

	{        // sync with previous read
		const VkBufferMemoryBarrier2 indexBarrier =
		    init::buffer_memory_barrier2(
		        VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
		        VK_ACCESS_2_MEMORY_READ_BIT,
		        VK_PIPELINE_STAGE_2_TRANSFER_BIT,
		        VK_ACCESS_2_MEMORY_WRITE_BIT,
		        _indexBuffer->handle()->handle());

		const VkBufferMemoryBarrier2 vertexBarrier =
		    init::buffer_memory_barrier2(
		        VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
		        VK_ACCESS_2_MEMORY_READ_BIT,
		        VK_PIPELINE_STAGE_2_TRANSFER_BIT,
		        VK_ACCESS_2_MEMORY_WRITE_BIT,
		        _vertexBuffer->handle()->handle());

		std::array<VkBufferMemoryBarrier2, 2> barrier = {indexBarrier, vertexBarrier};

		const VkDependencyInfo depInfo =
		    init::dependency_info({}, barrier);

		vkCmdPipelineBarrier2(cmd, &depInfo);
	}
	const uint32_t frameIndex       = instance.get_current_frame_index();
	size_t         currIndexOffset  = 0;
	size_t         currVertexOffset = 0;

	size_t totalIdxCount = 0;
	size_t totalVtxCount = 0;

	for (int i = 0; i < drawData->CmdListsCount; ++i)
	{
		totalIdxCount += drawData->CmdLists[i]->IdxBuffer.size();
		totalVtxCount += drawData->CmdLists[i]->VtxBuffer.size();
	}

	//	WVK_ASSERT_MSG(sizeof(ImDrawVert) * totalVtxCount <= _vertexBuffer.size(), "");
	//	WVK_ASSERT_MSG(sizeof(ImDrawIdx) * totalIdxCount <= _indexBuffer.size(), "");

	for (int i = 0; i < drawData->CmdListsCount; ++i)
	{
		const ImDrawList &cmdList = *drawData->CmdLists[i];

		_indexBuffer->upload(
		    cmd,
		    frameIndex,
		    cmdList.IdxBuffer.Data,
		    sizeof(ImDrawIdx) * cmdList.IdxBuffer.Size,
		    sizeof(ImDrawIdx) * currIndexOffset,
		    false);

		_vertexBuffer->upload(
		    cmd,
		    frameIndex,
		    cmdList.VtxBuffer.Data,
		    sizeof(ImDrawVert) * cmdList.VtxBuffer.Size,
		    sizeof(ImDrawVert) * currVertexOffset,
		    false);

		currIndexOffset += cmdList.IdxBuffer.Size;
		currVertexOffset += cmdList.VtxBuffer.Size;
	}
	{        // sync with previous read
		VkBufferMemoryBarrier2 indexBarrier =
		    init::buffer_memory_barrier2(
		        VK_PIPELINE_STAGE_2_COPY_BIT,
		        VK_ACCESS_2_MEMORY_WRITE_BIT,
		        VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
		        VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT,
		        _indexBuffer->handle()->handle());

		const VkBufferMemoryBarrier2 vertexBarrier =
		    init::buffer_memory_barrier2(
		        VK_PIPELINE_STAGE_2_COPY_BIT,
		        VK_ACCESS_2_MEMORY_WRITE_BIT,
		        VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
		        VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT,
		        _vertexBuffer->handle()->handle());

		std::array<VkBufferMemoryBarrier2, 2> barrier = {indexBarrier, vertexBarrier};

		const VkDependencyInfo depInfo =
		    init::dependency_info({}, barrier);

		vkCmdPipelineBarrier2(cmd, &depInfo);
	}
}

// ---------------------------------------------------------------------------------------------
// gfx::SpriteRenderingPipeline
// ---------------------------------------------------------------------------------------------

SpriteRenderingPipeline::SpriteRenderingPipeline(
    wvk::gfx::Instance &instance,
    VkFormat            drawImageFormat,
    size_t              maxSprites,
    std::string_view    name) :
    _debug_name("sprite_rendering_pipeline_" + std::string(name))
{
	auto device = instance.get_device();

	VkShaderModule vertShader = load_shader_module(
	    instance.get_device(),
	    "Builtin.Sprite.vert.glsl",
	    "sprite_shader_vert");

	VkShaderModule fragShader = load_shader_module(
	    instance.get_device(),
	    "Builtin.Sprite.frag.glsl",
	    "sprite_shader_frag");

	const std::array<VkDescriptorSetLayout, 1> descLayouts = {
	    instance.get_bindless_desc_set_layout()};

	const std::array<VkPushConstantRange, 1> bufferRanges = {
	    VkPushConstantRange{
	        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
	        .offset     = 0,
	        .size       = sizeof(PushConstants)}};

	_pipeline_layout = create_pipeline_layout(
	    device,
	    descLayouts,
	    bufferRanges,
	    "sprite_rendering_pipeline_layout_" + std::string(name));

	_pipeline =
	    PipelineBuilder(_pipeline_layout)
	        .set_name(_debug_name)
	        .set_shaders(vertShader, fragShader)
	        .set_input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
	        .set_polygon_mode(VK_POLYGON_MODE_FILL)
	        .disable_culling()
	        .set_multisampling_none()
	        .set_color_attachment_format(drawImageFormat)
	        .disable_depth_test()
	        .build(instance.get_device());

	vkDestroyShaderModule(instance.get_device(), vertShader, VK_NULL_HANDLE);
	vkDestroyShaderModule(instance.get_device(), fragShader, VK_NULL_HANDLE);

	for (size_t i = 0; i < FRAME_OVERLAP; ++i)
	{
		_frames[i].sprite_draw_command_buffer = instance.make_persistent_buffer(
		    maxSprites * sizeof(wvk::SpriteDrawCommand),
		    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		    "sprite draw commands");
	}
}

void SpriteRenderingPipeline::cleanup(
    Instance &instance)
{
	auto device = instance.get_device();

	for (size_t i = 0; i < FRAME_OVERLAP; ++i)
	{
		_frames[i].sprite_draw_command_buffer->cleanup();
	}

	vkDestroyPipelineLayout(device, _pipeline_layout, VK_NULL_HANDLE);
	vkDestroyPipeline(device, _pipeline, VK_NULL_HANDLE);
}

void SpriteRenderingPipeline::draw(
    VkCommandBuffer                           &cmd,
    Instance                                  &instance,
    const shared_ptr<Texture>                 &drawImage,
    const glm::mat4                           &viewProj,
    const std::vector<wvk::SpriteDrawCommand> &drawCommands)
{
	if (drawCommands.empty())
	{
		return;
	}

	auto &pfd = get_current_frame_data(instance.get_current_frame_index()).sprite_draw_command_buffer;
	pfd->copy_data_to_buffer(drawCommands.data(), drawCommands.size() * sizeof(wvk::SpriteDrawCommand));

	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);
	instance.bind_bindless_desc_set(cmd, _pipeline_layout);

	const auto viewport = VkViewport{
	    .x        = 0,
	    .y        = 0,
	    .width    = WVK_CAST(float, drawImage->get_extent_2d().width),
	    .height   = WVK_CAST(float, drawImage->get_extent_2d().height),
	    .minDepth = 0.0f,
	    .maxDepth = 1.0f};
	vkCmdSetViewport(cmd, 0, 1, &viewport);

	const auto scissor = VkRect2D{
	    .offset = {},
	    .extent = drawImage->get_extent_2d()};
	vkCmdSetScissor(cmd, 0, 1, &scissor);

	const RenderInfo renderInfo = create_render_info(
	    {.renderArea     = {{0}, drawImage->get_extent_2d()},
	     .colorImageView = drawImage->get_image_view()});

	vkCmdBeginRendering(cmd, &renderInfo.info);

	const auto pc = PushConstants{
	    .view_proj     = viewProj,
	    .commandBuffer = pfd->get_device_address(instance.get_device())};

	vkCmdPushConstants(
	    cmd,
	    _pipeline_layout,
	    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
	    0,
	    sizeof(PushConstants),
	    &pc);

	vkCmdDraw(
	    cmd,
	    6,
	    WVK_CAST(uint32_t, drawCommands.size()),
	    0,
	    0);

	vkCmdEndRendering(cmd);
}
}        // namespace wvk::gfx
