#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <volk.h>

#include "wvk/common/wvk_defines.hpp"

namespace wvk
{
struct MeshDrawCommand;
}

namespace wvk::core
{
class Transform;
}

namespace wvk::gfx
{
class Instance;
class MeshCache;
class MaterialCache;

class MeshPipeline;
class CSMShadowMapPipeline;
class PointLightShadowMapPipeline;
class SpotLightShadowMapPipeline;

class NBuffer;

struct LightData;
struct Light;

class WVK_API DeferredRenderer
{
  public:
	explicit DeferredRenderer(
	    Instance                        &instance,
	    const shared_ptr<MeshCache>     &meshCache,
	    const shared_ptr<MaterialCache> &materialCache,
	    const glm::vec2                 &renderExtent);

	void cleanup(
	    Instance &instance);

	void begin_draw();

	void end_draw();

	void draw(
	    VkCommandBuffer &cmd,
	    Instance        &instance);

	void draw_light(
	    const Light           &light,
	    const core::Transform &transform);

	void draw_mesh(
	    MeshId           id,
	    MaterialId       materialId,
	    bool             castShadow,
	    const glm::mat4 &transform);

  private:
	struct SceneData
	{
		// camera data
		glm::mat4 view;
		glm::mat4 proj;
		glm::mat4 viewProj;
		glm::vec4 cameraPos;

		// ambient data
		ColorRGB ambientColor;
		float    ambientIntensity;

		// fog data
		ColorRGB fogColor;
		float    fogIntensity;

		// csm data
		glm::vec4                                           cascadeFarPlaneZs;
		std::array<glm::mat4, WVK_CSM_SHADOW_CASCADE_COUNT> csmLightSpaceTMs;
		uint32_t                                            csmShadowMapId;

		// point light data
		float pointLightFarPlane;

		// light data
		VkDeviceAddress lightsBufferAddr;
		uint32_t        lightCount;
		uint32_t        sunlightIndex;

		// material data
		VkDeviceAddress materialsBufferAddr;
	};

	shared_ptr<MeshCache>     _mesh_cache;
	shared_ptr<MaterialCache> _material_cache;

	unique_ptr<MeshPipeline> _mesh_pipeline;
	//	unique_ptr<CSMShadowMapPipeline>        _csm_shadow_map_pipeline;
	//	unique_ptr<SpotLightShadowMapPipeline>  _spot_light_shadow_map_pipeline;
	//	unique_ptr<PointLightShadowMapPipeline> _point_light_shadow_map_pipeline;

	shared_ptr<NBuffer> _scene_data_buffer;
	shared_ptr<NBuffer> _light_data_buffer;

	std::vector<LightData> _light_data;
	const float            _point_light_max_range = WVK_POINT_LIGHT_MAX_RANGE;
	const float            _spot_light_max_range  = WVK_SPOT_LIGHT_MAX_RANGE;
	int32_t                _sunlight_index        = WVK_SUN_LIGHT_INDEX;

	std::vector<MeshDrawCommand> _mesh_draw_commands;
	std::vector<size_t>          _sorted_mesh_draw_commands;

	VkFormat _draw_texture_format  = VK_FORMAT_R16G16B16A16_SFLOAT;
	VkFormat _depth_texture_format = VK_FORMAT_D32_SFLOAT;

	TextureId _draw_texture_id          = NULL_TEXTURE_ID;
	TextureId _resolve_draw_texture_id  = NULL_TEXTURE_ID;
	TextureId _depth_texture_id         = NULL_TEXTURE_ID;
	TextureId _resolve_depth_texture_id = NULL_TEXTURE_ID;

	VkSampleCountFlagBits _sample = VK_SAMPLE_COUNT_1_BIT;

	bool _shadows_enabled = true;

  private:
	void _sort_draw_list();
};

}        // namespace wvk::gfx