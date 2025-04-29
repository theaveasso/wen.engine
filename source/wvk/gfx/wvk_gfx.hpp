#pragma once

#include <VkBootstrap.h>
#include <glm/mat4x4.hpp>

#include "wvk/common/wvk_defines.hpp"
#include "wvk/common/wvk_pch.hpp"
#include "wvk/core/wvk_core.hpp"

#include "wvk_pipeline.gfx.hpp"
#include "wvk_types.gfx.hpp"

struct GLFWwindow;

namespace wvk::core
{
class LdtkJson;
class LdtkDefs;
class LdtkLevel;
class LdtkLayer;
}        // namespace wvk::core

namespace wvk::gfx
{

class Texture;
class Buffer;
class NBuffer;

// ---------------------------------------------------------------------------------------------
// gfx::CubeMap
// ---------------------------------------------------------------------------------------------
shared_ptr<Texture> loadCubeMap(
    Instance                    &instance,
    const std::filesystem::path &imageDir);

// ---------------------------------------------------------------------------------------------
// gfx::ImageData
// ---------------------------------------------------------------------------------------------
struct WVK_API ImageData
{
	ImageData() = default;
	~ImageData();
	WVK_MOVABLE_ONLY(ImageData);

	unsigned char *pixels   = nullptr;
	int            width    = 0;
	int            height   = 0;
	int            channels = 0;
};

WVK_API void load_image(
    const std::filesystem::path &path,
    ImageData                   *image);

// ---------------------------------------------------------------------------------------------
// gfx::Light
// ---------------------------------------------------------------------------------------------

enum class LightType
{
	NONE,
	DIRECTIONAL,
	POINT,
	SPOT
};

struct Light
{
	ColorRGBA color       = WHITE;
	LightType type        = LightType::NONE;
	float     range       = 0.0f;
	float     intensity   = 0.0f;
	glm::vec2 scaleOffset = glm::vec2{0};        // precomputed for spotlights (for angle attenuation)
	bool      cast_shadow = true;

	std::string name;
};

// Representation of light data on GPU (Builtin.Lighting.glsl)
struct LightData
{
	glm::vec3     position;
	glm::vec3     direction;
	float         range;
	std::uint32_t type;
	ColorRGB      color;
	float         intensity;
	glm::vec2     scaleOffset;
	TextureId     shadowMapId;
	float         unused;
};

WVK_API inline void set_cone_angles(
    float  innerConeAngle,
    float  outerConeAngle,
    Light *light)
{
}

void get_gpu_type(LightType type);

// ---------------------------------------------------------------------------------------------
// gfx::Material
// ---------------------------------------------------------------------------------------------
struct MaterialData
{
	ColorRGBA baseColor;
	uint32_t  diffuseTex;
	uint32_t  normalTex;
	uint32_t  emissiveTex;
	uint32_t  metallicRoughnessTex;
	glm::vec4 metalRoughnessEmissive;
};

struct Material
{
	std::string name;

	ColorRGBA baseColor;
	float     emissiveFactor  = 0.0f;
	float     roughnessFactor = 0.0f;
	float     metallicFactor  = 0.0f;

	TextureId diffuseTexture           = NULL_TEXTURE_ID;
	TextureId normalMapTexture         = NULL_TEXTURE_ID;
	TextureId emissiveTexture          = NULL_TEXTURE_ID;
	TextureId metallicRoughnessTexture = NULL_TEXTURE_ID;
};

class WVK_API MaterialCache
{
  public:
	MaterialCache() = default;
	explicit MaterialCache(Instance &instance);

	void cleanup();

	MaterialId      add_material(Instance &instance, Material material);
	const Material &get_material(MaterialId id) { return _materials[id]; }

	MaterialId get_free_material_id() const { return WVK_CAST(MaterialId, _materials.size()); }
	MaterialId get_placeholder_material_id() const;

	[[nodiscard]] const shared_ptr<Buffer> &get_material_data_buffer() const { return _materialDataBuffer; }

	VkDeviceAddress get_material_data_buffer_address(VkDevice device) const;

  private:
	static const auto MAX_MATERIALS = 1000;

	std::vector<Material> _materials;

	shared_ptr<Buffer> _materialDataBuffer;
	MaterialId _placeholder_material_id       = NULL_MATERIAL_ID;
	TextureId  _default_normal_map_texture_id = NULL_TEXTURE_ID;
};

// ---------------------------------------------------------------------------------------------
// gfx::Mesh
// ---------------------------------------------------------------------------------------------
struct CPUMesh
{
	struct Vertex
	{
		glm::vec3 position;
		float     uv_x;
		glm::vec3 normal;
		float     uv_y;
		glm::vec4 tangent;
	};

	glm::vec3 minPos, maxPos;

	std::vector<uint32_t> indices;
	std::vector<Vertex> vertices;

	std::string name;
};

struct GPUMesh
{
	shared_ptr<Buffer> vertexBuffer;
	shared_ptr<Buffer> indexBuffer;
	shared_ptr<Buffer> skinningDataBuffer;

	uint32_t numVertices = 0;
	uint32_t numIndices  = 0;

	glm::vec3 minPos;
	glm::vec3 maxPos;

	bool   hasSkeleton = false;
};

class WVK_API MeshCache
{
  public:
	explicit MeshCache(Instance &instance);
	void cleanup();

	MeshId         add_mesh(Instance &instance, CPUMesh &mesh);
	const GPUMesh &get_gpu_mesh(MeshId id);

  private:
	std::vector<GPUMesh> _meshes;

	void upload_mesh(
	    Instance                  &instance,
	    const shared_ptr<CPUMesh> &cpuMesh,
	    const shared_ptr<GPUMesh> &gpuMesh) const;
};

struct SkinnedMesh
{
	shared_ptr<Buffer> skinnedVertexBuffer;
};

// class WVK_API SceneCache
//{
//   public:
//	SceneCache() = default;
//
//	explicit SceneCache(
//	    Instance      &instance,
//	    MeshCache     &meshCache,
//	    MaterialCache &materialCache);
//
//	void cleanup(
//	    Instance &instance);
//
//	const Scene &add_scene(
//	    const std::string &path,
//	    Scene              scene);
//
//	const Scene &get_scene(
//	    const std::string &path);
//
//	const Scene &load_or_get_scene(
//	    const std::filesystem::path &path);
//
//   private:
//	std::unordered_map<std::string, Scene> _scenes;
//
//	Instance     &_instance;
//	MeshCache    &_mesh_cache;
//	MaterialCache _material_cache;
// };

// WVK_API const Scene &scene_load(
//     Instance                    &instance,
//     MeshCache                   &meshCache,
//     MaterialCache               &materialCache,
//     const std::filesystem::path &path);
//
// ---------------------------------------------------------------------------------------------
// gfx::Sprite
// ---------------------------------------------------------------------------------------------
#pragma region gfx::Sprite
struct WVK_API Sprite
{
  public:
	Sprite() = default;
	explicit Sprite(const std::shared_ptr<Texture> &texture);

	uint32_t                 texture_id = 0;
	glm::vec2                texture_size;

	glm::vec2 uv0 = glm::vec2{0.f};
	glm::vec2 uv1 = glm::vec2{1.f};

	glm::vec2 pivot = glm::vec2{0.f};
	glm::vec4 color = glm::vec4{1.f, 1.f, 1.f, 1.f};

	void set_texture(const std::shared_ptr<Texture> &texture);

	void set_texture_size(glm::vec2 size);

	void set_texture_rect(const core::Rectangle &rec);

	void set_pivot_pixel(const glm::ivec2 &pixel);
};
#pragma endregion

// ---------------------------------------------------------------------------------------------
// gfx::SpriteRenderer
// ---------------------------------------------------------------------------------------------

#pragma region gfx::SpriteRenderer
class WVK_API SpriteRenderer
{
  public:
	static constexpr uint32_t SPRITE_SHADER_ID  = 0;
	static constexpr uint32_t TEXTURE_SHADER_ID = 1;

  public:
	SpriteRenderer() = default;

	void init(
	    Instance        &instance,
	    VkFormat         drawTextureFormat,
	    std::string_view name = "");

	void cleanup(
	    Instance &instance);

	void begin_draw();

	void end_draw();

	void draw(
	    VkCommandBuffer           &cmd,
	    Instance                  &instance,
	    const shared_ptr<Texture> &drawImage);

	void draw(
	    VkCommandBuffer           &cmd,
	    Instance                  &instance,
	    const shared_ptr<Texture> &drawImage,
	    const glm::mat4           &viewProjection);

	void draw_sprite(
	    const Sprite    &sprite,
	    const glm::vec2 &position,
	    float            rotation,
	    const glm::vec2 &scale,
	    uint32_t         shaderId = SPRITE_SHADER_ID);

	void draw_sprite(
	    const Sprite    &sprite,
	    const glm::mat4 &transform,
	    uint32_t         shaderId = SPRITE_SHADER_ID);

	void draw_text();        // TODO

	void draw_rectangle(
	    Instance        &instance,
	    int              posX,
	    int              posY,
	    int              width,
	    int              height,
	    const glm::vec4 &color);

	void draw_rectangle_v(
	    Instance        &instance,
	    const glm::vec2 &position,
	    const glm::vec2 &size,
	    const glm::vec4 &color);

	void draw_rectangle_rec(
	    Instance              &instance,
	    const core::Rectangle &rec,
	    const glm::vec4       &color);

	void draw_rectangle_pro(
	    Instance              &instance,
	    const core::Rectangle &rec,
	    const glm::vec2       &origin,
	    float                  rotation,
	    const glm::vec4       &color);

	// draw a texture 2d
	void draw_texture(
	    shared_ptr<Texture> texture,
	    int                 posX,
	    int                 posY,
	    const ColorRGBA    &tint = WHITE);

	// draw a texture 2d with position defined as glm::vec2
	void draw_texture_v(
	    shared_ptr<Texture> texture,
	    glm::vec2           position,
	    const ColorRGBA    &tint = WHITE);

	// draw a texture 2d with extended parameters
	void draw_texture_ex(
	    shared_ptr<Texture> texture,
	    glm::vec2           position,
	    float               rotation,
	    float               scale,
	    const ColorRGBA    &tint = WHITE);

	// draw a part of a texture defined by a rectangle
	void draw_texture_rec(
	    shared_ptr<Texture>    texture,
	    const core::Rectangle &source,
	    glm::vec2              position,
	    const ColorRGBA       &tint = WHITE);

	// draw a part of a texture defined by a rectangle with 'pro' parameters
	void draw_texture_pro(
	    shared_ptr<Texture>    texture,
	    const core::Rectangle &source,
	    const core::Rectangle &dest,
	    glm::vec2              origin,
	    float                  rotation,
	    const ColorRGBA       &tint = WHITE);

  private:
	bool _is_initialized = false;

	SpriteRenderingPipeline _drawing_pipeline;

	static const size_t            MAX_SPRITES           = 25000;
	std::vector<SpriteDrawCommand> _sprite_draw_commands = {};
};

#pragma endregion

// ---------------------------------------------------------------------------------------------
// gfx::Texture
// ---------------------------------------------------------------------------------------------

class WVK_API TextureCache
{
  public:
	explicit TextureCache(Instance &instance);
	void cleanup();

	TextureId      add_texture(Instance &instance, Texture material);
	const shared_ptr<Texture> &get_texture(TextureId id);

	TextureId get_free_texture_id() const;

  private:
	std::vector<shared_ptr<Texture>> textures;
};

// ---------------------------------------------------------------------------------------------
// gfx::TileMapRenderer
// ---------------------------------------------------------------------------------------------

class WVK_API TileMapRenderer
{
  public:
	static void draw_level(
	    Instance              &instance,
	    SpriteRenderer        &spriteRenderer,
	    const core::LdtkLevel &level);

	static void draw_layer(
	    Instance              &instance,
	    SpriteRenderer        &spriteRenderer,
	    const core::LdtkLayer &layer);
};

// ---------------------------------------------------------------------------------------------
// gfx::UIOverlay
// ---------------------------------------------------------------------------------------------

class WVK_API UIOverlay
{
  public:
	explicit UIOverlay(
	    Instance        &instance,
	    GLFWwindow      *window,
	    VkFormat         drawImageFormat,
	    std::string_view name = "");

	void cleanup(
	    Instance &instance);

	void begin_draw();

	void end_draw();

	void draw(
	    VkCommandBuffer cmd,
	    Instance       &instance,
	    VkImageView     view,
	    VkExtent2D      extent);

	ImGuiContext *get_context() { return _context; }

  private:
	bool _is_initialized = false;

	ImGuiContext *_context;
	UIOverlayPipeline _drawing_pipeline;
};
}        // namespace wvk::gfx
