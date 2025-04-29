#include "wvk_gfx.hpp"
#include "wvk_pipeline.gfx.hpp"

#include "wvk_deferred.gfx.hpp"

#include "wvk/core/wvk_math.hpp"

#include "wvk/gfx/wvk_types.gfx.hpp"
#include "wvk_inst.gfx.hpp"
#include "wvk_vulkan.gfx.hpp"

#include <numeric>

namespace wvk::gfx
{

DeferredRenderer::DeferredRenderer(
    wvk::gfx::Instance              &instance,
    const shared_ptr<MeshCache>     &meshCache,
    const shared_ptr<MaterialCache> &materialCache,
    const glm::vec2                 &renderExtent) :
    _mesh_cache(meshCache), _material_cache(materialCache)
{
	_light_data.resize(WVK_MAX_LIGHT_COUNT);

	_scene_data_buffer =
	    make_shared<NBuffer>(
	        instance,
	        sizeof(SceneData),
	        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
	        FRAME_OVERLAP);

	_light_data_buffer =
	    make_shared<NBuffer>(
	        instance,
	        sizeof(LightData) * WVK_MAX_LIGHT_COUNT,
	        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
	        FRAME_OVERLAP);

	instance.get_max_supported_sampling_count();

	const auto extent = VkExtent3D{
	    WVK_CAST(uint32_t, renderExtent.x),
	    WVK_CAST(uint32_t, renderExtent.y),
	    1};

	const auto drawTextureInfo =
	    CreateTextureInfo{
	        .format  = _draw_texture_format,
	        .usages  = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
	        .extent  = extent,
	        .sampler = _sample};

	_draw_texture_id = instance.make_texture(
	    drawTextureInfo,
	    "deferred_draw_texture");

	_resolve_draw_texture_id = instance.make_texture(
	    drawTextureInfo,
	    "deferred_resolve_draw_texture");

	auto depthTextureInfo =
	    CreateTextureInfo{
	        .format  = _depth_texture_format,
	        .usages  = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
	        .extent  = extent,
	        .sampler = _sample};

	_depth_texture_id = instance.make_texture(
	    depthTextureInfo,
	    "deferred_depth_texture");

	depthTextureInfo.sampler  = VK_SAMPLE_COUNT_1_BIT;        // no MSAA
	_resolve_depth_texture_id = instance.make_texture(
	    depthTextureInfo,
	    "deferred_depth_resolve_texture");

	_mesh_pipeline =
	    make_unique<MeshPipeline>(
	        instance,
	        _draw_texture_format,
	        _depth_texture_format,
	        _sample);

	//	_csm_shadow_map_pipeline =
	//	    make_unique<CSMShadowMapPipeline>(
	//	        instance,
	//	        std::array{0.138f, 0.35f, 1.f});
	//
	//	_point_light_shadow_map_pipeline =
	//	    make_unique<PointLightShadowMapPipeline>(
	//	        instance,
	//	        _point_light_max_range);
	//
	//	_spot_light_shadow_map_pipeline =
	//	    make_unique<SpotLightShadowMapPipeline>(
	//	        instance);
}

void DeferredRenderer::cleanup(
    wvk::gfx::Instance &instance)
{
	_mesh_pipeline->cleanup(instance);
	_mesh_pipeline.reset();
	//	_csm_shadow_map_pipeline->cleanup(instance);
	//	_csm_shadow_map_pipeline.reset();
	//	_point_light_shadow_map_pipeline->cleanup(instance);
	//	_point_light_shadow_map_pipeline.reset();
	//	_spot_light_shadow_map_pipeline->cleanup(instance);
	//	_spot_light_shadow_map_pipeline.reset();

	_scene_data_buffer->cleanup();
	_scene_data_buffer.reset();

	_light_data_buffer->cleanup();
	_light_data_buffer.reset();
}

void DeferredRenderer::begin_draw()
{
	_mesh_draw_commands.clear();
	_light_data.clear();
	_sunlight_index = -1;
}

void DeferredRenderer::end_draw()
{
	_sort_draw_list();
}

void DeferredRenderer::draw(
    VkCommandBuffer    &cmd,
    wvk::gfx::Instance &instance)
{
	//	auto device = instance.get_device();

	//	if (_sunlight_index != -1)
	//	{
	//	cmd_begin_w_label(cmd, "csm");

	//	auto &sunlight = _light_data[_sunlight_index];
	//		_csm_shadow_map_pipeline->draw(
	//		    cmd,
	//		    instance,
	//		    _mesh_cache,
	//		    _mesh_draw_commands,
	//		    _shadows_enabled);

	//	cmd_end_w_label(cmd);
	//	}

	WVK_PROFILER_ZONE("point_shadow", tracy::Color::CornflowerBlue);
	WVK_PROFILER_GPU_ZONE("point_shadow", instance.get_tracy_vk_ctx(), cmd, tracy::Color::CornflowerBlue)
	cmd_begin_w_label(cmd, "point_shadow");

	//	std::vector<size_t> pointLightIndices;
	//	for (size_t i = 0; i < _light_data.size(); ++i)
	//	{
	//	}
	//
	//	_point_light_shadow_map_pipeline->begin_draw(
	//	    cmd,
	//	    instance,
	//	    _light_data,
	//	    pointLightIndices);
	//
	//	for (const auto lightIndex : pointLightIndices)
	//	{
	//		const auto &light = _light_data[lightIndex];
	//		_point_light_shadow_map_pipeline->draw(
	//		    cmd,
	//		    instance,
	//		    _mesh_cache,
	//		    lightIndex,
	//		    _material_cache.get_material_data_buffer(),
	//		    light.position,
	//		    _light_data_buffer->handle(),
	//		    _mesh_draw_commands,
	//		    _shadows_enabled);
	//	}
	//
	//	_point_light_shadow_map_pipeline->end_draw(cmd, instance);

	cmd_end_w_label(cmd);
	WVK_PROFILER_ZONE_END();

	const auto &drawTexture  = instance.get_texture(_draw_texture_id);
	const auto &depthTexture = instance.get_texture(_depth_texture_id);

	WVK_PROFILER_ZONE("geometry", tracy::Color::ForestGreen);
	WVK_PROFILER_GPU_ZONE("geometry", instance.get_tracy_vk_ctx(), cmd, tracy::Color::ForestGreen)
	cmd_begin_w_label(cmd, "geometry");
	transition_image(cmd, drawTexture->handle(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	transition_image(cmd, depthTexture->handle(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

	const auto renderingInfo = RenderingInfo{
	    .renderArea           = {.offset = {}, .extent = drawTexture->get_extent_2d()},
	    .colorImageView       = drawTexture->get_image_view(),
	    .depthImageView       = depthTexture->get_image_view(),
	    .colorImageClearValue = {{0.0f, 0.0f, 0.0f, 1.0f}},
	    .depthImageClearValue = 0.0f};

	const RenderInfo renderInfo = create_render_info(renderingInfo);

	vkCmdBeginRendering(cmd, &renderInfo.info);

	_mesh_pipeline->draw(
	    cmd,
	    drawTexture->get_extent_2d(),
	    instance,
	    _mesh_cache,
	    _material_cache,
	    _scene_data_buffer->handle(),
	    _mesh_draw_commands,
	    _sorted_mesh_draw_commands);

	vkCmdEndRendering(cmd);

	cmd_end_w_label(cmd);
	WVK_PROFILER_ZONE_END();
}

void DeferredRenderer::draw_light(
    const Light           &light,
    const core::Transform &transform)
{
}

void DeferredRenderer::draw_mesh(
    MeshId           id,
    MaterialId       materialId,
    bool             castShadow,
    const glm::mat4 &transform)
{
	_mesh_draw_commands.push_back(
	    MeshDrawCommand{
	        .transform  = transform,
	        .meshId     = id,
	        .materialId = materialId,
	        .castShadow = castShadow});
}

void DeferredRenderer::_sort_draw_list()
{
	_sorted_mesh_draw_commands.clear();
	_sorted_mesh_draw_commands.resize(_mesh_draw_commands.size());

	std::iota(_sorted_mesh_draw_commands.begin(), _sorted_mesh_draw_commands.end(), 0);

	std::sort(
	    _sorted_mesh_draw_commands.begin(),
	    _sorted_mesh_draw_commands.end(),
	    [this](const auto &i1, const auto &i2) {
		    const auto &drawCommand1 = _mesh_draw_commands[i1];
		    const auto &drawCommand2 = _mesh_draw_commands[i2];
		    return drawCommand1.meshId < drawCommand2.meshId;
	    });
}

}        // namespace wvk::gfx
