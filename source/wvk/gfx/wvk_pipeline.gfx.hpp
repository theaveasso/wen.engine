#pragma once

#include <glm/mat4x4.hpp>

#include "wvk/common/wvk_pch.hpp"
#include "wvk_types.gfx.hpp"

namespace wvk::gfx
{

class Instance;

class Buffer;
class NBuffer;

class Texture;

struct Camera;
class MeshCache;
class MaterialCache;

struct LightData;
struct Light;

class WVK_API CSMShadowMapPipeline
{
  public:
	explicit CSMShadowMapPipeline(
	    Instance                                              &instance,
	    const std::array<float, WVK_CSM_SHADOW_CASCADE_COUNT> &percents,
	    std::string_view                                       name = "");

	void cleanup(
	    Instance &instance);

	void draw(
	    VkCommandBuffer                     cmd,
	    Instance                           &instance,
	    MeshCache                          &meshCache,
	    const std::vector<MeshDrawCommand> &drawCommands,
	    bool                                shadowEnabled = false);

	[[nodiscard]] TextureId get_shadow_map() const { return _csm_shadow_map_id; }

  private:
	struct PushConstants
	{
		glm::mat4       mvp;
		VkDeviceAddress vertexBuffer{};
		VkDeviceAddress materialsBuffer{};
		MaterialId      materialId{};
	};

	VkPipeline       _pipeline;
	VkPipelineLayout _pipeline_layout;

	TextureId _csm_shadow_map_id = NULL_TEXTURE_ID;

	float _shadow_map_texture_size = 4096.f;

	std::array<float, NUM_SHADOW_CASCADES>       _cascade_far_plane_z{};
	std::array<glm::mat4, NUM_SHADOW_CASCADES>   _csm_light_space_t;
	std::array<float, NUM_SHADOW_CASCADES>       _percents{};
	std::array<VkImageView, NUM_SHADOW_CASCADES> _csm_shadow_map_views{};
};

class WVK_API SpotLightShadowMapPipeline
{
  public:
	explicit SpotLightShadowMapPipeline(
	    Instance        &instance,
	    std::string_view name = "");
	void cleanup(Instance &instance);

  private:
	struct PushConstants
	{
	};

	VkPipeline       _pipeline;
	VkPipelineLayout _pipeline_layout;
};

class WVK_API PointLightShadowMapPipeline
{
  public:
	explicit PointLightShadowMapPipeline(
	    Instance        &instance,
	    float            pointLightMaxRange,
	    std::string_view name = "");

	void cleanup(Instance &instance);

	void begin_draw(
	    VkCommandBuffer               cmd,
	    const Instance               &instance,
	    const std::vector<LightData> &lightData,
	    const std::vector<size_t>    &pointingLightIndices);

	void end_draw(
	    VkCommandBuffer cmd,
	    Instance       &instance);

	void draw(
	    VkCommandBuffer                     cmd,
	    Instance                           &instance,
	    MeshCache                          &meshCache,
	    LightId                             lightId,
	    const shared_ptr<Buffer>           &materialsBuffer,
	    const glm::vec3                    &lightPosition,
	    const shared_ptr<Buffer>           &lightsBuffer,
	    const std::vector<MeshDrawCommand> &drawCommands,
	    bool                                shadowsEnabled = false);

  private:
	struct PushConstants
	{
		glm::mat4       model;
		VkDeviceAddress vertexBuffer{};
		VkDeviceAddress lightsBuffer{};
		VkDeviceAddress materialsBuffer{};
		MaterialId      materialId{};
		LightId         lightId{};
		uint32_t        vpsBufferIndex;
		float           farPlane;
	};

	VkPipeline       _pipeline;
	VkPipelineLayout _pipeline_layout;

	float _point_light_max_range   = 0.0f;
	float _shadow_map_texture_size = 1024.f;

	static constexpr int MAX_POINT_LIGHTS = 32;

	std::array<glm::mat4, MAX_POINT_LIGHTS * 6> _point_light_shadow_map_vps;

	std::array<TextureId, MAX_POINT_LIGHTS> _shadow_maps{};
	std::vector<VkImageView>                _shadow_map_image_views;

	std::unordered_map<LightId, TextureId> _light_to_shadow_map_id;
	TextureId                              _current_shadow_map_index = 0;

	shared_ptr<NBuffer> _view_projection_buffer;
};

// ---------------------------------------------------------------------------------------------
// gfx::MeshPipeline
// ---------------------------------------------------------------------------------------------

/// @brief Simple graphics pipeline for rendering static meshes.
class WVK_API MeshPipeline
{
  public:
	MeshPipeline() = default;

	/// Initializes the MeshPipeline, loading shaders and building the graphics pipeline.
	/// @param instance: Engine instance
	/// @param textureFormat: Color attachment format
	/// @param depthFormat: Depth attachment format
	/// @param samples: MSAA sample count
	/// @param name: Pipeline name suffix for debugging
	explicit MeshPipeline(
	    Instance             &instance,
	    VkFormat              textureFormat,
	    VkFormat              depthFormat,
	    VkSampleCountFlagBits samples,
	    std::string_view      name = "");

	/// Cleanup resources for the mesh pipeline.
	/// @param instance - Engine instance
	void cleanup(Instance &instance);

	/// Issue draw calls for a batch of mesh draw commands.
	/// @param cmd: active command buffer
	/// @param renderExtent: size of the render target
	/// @param instance: engine instance
	/// @param meshCache: mesh resource cache
	/// @param materialCache: material resource cache (not used here but can be for future extensions)
	/// @param sceneDataBuffer: buffer containing per-scene GPU data
	/// @param drawCommands: all draw commands
	/// @param sortedDrawCommands: pre-sorted indices into drawCommands for render ordering
	void draw(
	    VkCommandBuffer                     cmd,
	    VkExtent2D                          renderExtent,
	    Instance                           &instance,
	    const shared_ptr<MeshCache>        &meshCache,
	    const shared_ptr<MaterialCache>    &materialCache,
	    const shared_ptr<Buffer>           &sceneDataBuffer,
	    const std::vector<MeshDrawCommand> &drawCommands,
	    const std::vector<size_t>          &sortedDrawCommands);

  private:
	/// brief Small data packet sent to GPU shaders every draw call.
	struct PushConstants
	{
		glm::mat4       transform;                ///< Object's world transform matrix (model matrix)
		VkDeviceAddress sceneDataBuffer{};        ///< GPU address of the scene uniform buffer
		VkDeviceAddress vertexBuffer{};           ///< GPU address of the mesh's vertex buffer
		uint32_t        materialId{};             ///< Material ID for selecting material in bindless textures
		std::uint32_t   padding{};
	};

	VkPipeline       _pipeline;
	VkPipelineLayout _pipeline_layout;
};

// ---------------------------------------------------------------------------------------------
// gfx::SpriteRenderingPipeline
// ---------------------------------------------------------------------------------------------
#pragma region gfx::SpriteRenderingPipeline
class WVK_API  SpriteRenderingPipeline final
{
  public:
	SpriteRenderingPipeline() = default;
	explicit SpriteRenderingPipeline(
	    Instance        &instance,
	    VkFormat         drawImageFormat,
	    size_t           maxSprites,
	    std::string_view name = "");

	void cleanup(Instance &instance);

	void draw(
	    VkCommandBuffer                      &cmd,
	    Instance                             &instance,
	    const shared_ptr<Texture>            &drawImage,
	    const glm::mat4                      &viewProj,
	    const std::vector<SpriteDrawCommand> &drawCommands);

  private:
	struct PushConstants
	{
		glm::mat4       view_proj;
		VkDeviceAddress commandBuffer = 0;
	};

	struct PerframeData
	{
		shared_ptr<Buffer> sprite_draw_command_buffer;
	};

	std::string _debug_name;

	VkPipelineLayout _pipeline_layout = VK_NULL_HANDLE;
	VkPipeline       _pipeline        = VK_NULL_HANDLE;

	std::array<PerframeData, FRAME_OVERLAP> _frames;

	PerframeData &get_current_frame_data(uint32_t frameIndex) { return _frames[frameIndex]; }

  private:
	void _copy_buffers(VkCommandBuffer cmd, Instance &instance) const;
};

// ---------------------------------------------------------------------------------------------
// gfx::UIOverlayPipeline
// ---------------------------------------------------------------------------------------------

class WVK_API UIOverlayPipeline final
{
  public:
	UIOverlayPipeline() = default;
	explicit UIOverlayPipeline(
	    Instance        &instance,
	    VkFormat         drawImageFormat,
	    std::string_view name = "");

	void cleanup(VkDevice device);

	void draw(
	    VkCommandBuffer &cmd,
	    Instance        &instance,
	    VkImageView      imageView,
	    VkExtent2D       extent);

  private:
	struct PushConstants
	{
		VkDeviceAddress vertexBuffer;
		uint32_t        textureId;
		uint32_t        textureIsSRGB;
		glm::vec2       translate;
		glm::vec2       scale;
	};

	std::string _debug_name;

	VkPipelineLayout _pipeline_layout = VK_NULL_HANDLE;
	VkPipeline       _pipeline        = VK_NULL_HANDLE;

	shared_ptr<NBuffer> _indexBuffer;
	shared_ptr<NBuffer> _vertexBuffer;

	TextureId _font_texture_id = NULL_TEXTURE_ID;

  private:
	void _copy_buffers(VkCommandBuffer cmd, Instance &instance);
};

#pragma endregion

}        // namespace wvk::gfx
