#pragma once

#include <VkBootstrap.h>
#include <glm/mat4x4.hpp>

#include "wvk/common/wvk_defines.hpp"
#include "wvk/common/wvk_pch.hpp"
#include "wvk/core/wvk_core.hpp"

#include "wvk_gfxpipeline.hpp"
#include "wvk_gfxtypes.hpp"
#include "wvk_gfxvulkan.hpp"

struct GLFWwindow;

namespace wvk::gfx
{

using ColorRGBA = glm::vec4;
using ColorRGB  = glm::vec3;

constexpr ColorRGBA WHITE = ColorRGBA{1.0f, 1.0f, 1.0f, 1.0f};
constexpr ColorRGBA BLACK = ColorRGBA{0.0f, 0.0f, 0.0f, 1.0f};

// ---------------------------------------------------------------------------------------------
// gfx::CubeMap
// ---------------------------------------------------------------------------------------------
Texture loadCubeMap(Instance &instance, const std::filesystem::path &imageDir);

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

void load_image(const std::filesystem::path &path, ImageData *image);

// ---------------------------------------------------------------------------------------------
// gfx::GameRenderer
// ---------------------------------------------------------------------------------------------

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

void set_cone_angles(float innerConeAngle, float outerConeAngle, Light *light);
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
	ColorRGBA baseColor;
	float     emissiveFactor  = 0.0f;
	float     roughnessFactor = 0.0f;
	float     metallicFactor  = 0.0f;

	TextureId diffuseTexture           = NULL_TEXTURE_ID;
	TextureId normalMapTexture         = NULL_TEXTURE_ID;
	TextureId emissiveTexture          = NULL_TEXTURE_ID;
	TextureId metallicRoughnessTexture = NULL_TEXTURE_ID;

	std::string name;
};

class MaterialCache
{
  public:
	explicit MaterialCache(Instance &instance);
	void cleanup();

	MaterialId      add_material(Instance &instance, Material material);
	const Material &get_material(MaterialId id);

	MaterialId get_free_material_id() const;
	MaterialId get_placeholder_material_id() const;

	const Buffer   &get_material_data_buffer() const;
	VkDeviceAddress get_material_data_buffer_address(VkDevice device) const;

  private:
	static const auto MAX_MATERIALS = 1000;

	std::vector<Material> _materials;

	Buffer     _materialDataBuffer;
	MaterialId _placeholder_material_id       = NULL_MATERIAL_ID;
	TextureId  _default_normal_map_texture_id = NULL_TEXTURE_ID;
};

// ---------------------------------------------------------------------------------------------
// gfx::Mesh
// ---------------------------------------------------------------------------------------------
struct CPUMesh
{
	glm::vec3 minPos, maxPos;

	std::vector<uint32_t> indices;

	struct Vertex
	{
		glm::vec3 position;
		float     uv_x;
		float     uv_y;
		glm::vec2 normal;
		glm::vec4 tangent;
	};
	std::vector<Vertex> vertices;

	struct SkinningData
	{
		glm::vec<4, uint32_t> jointIds;
		glm::vec4             weight;
	};

	std::vector<SkinningData> skinningData;

	bool hasSkeleton = false;

	std::string name;
};

struct GPUMesh
{
	Buffer vertexBuffer;
	Buffer indexBuffer;

	uint32_t numVertices = 0;
	uint32_t numIndices  = 0;

	glm::vec3 minPos;
	glm::vec3 maxPos;

	bool   hasSkeleton = false;
	Buffer skinningDataBuffer;
};

class MeshCache
{
  public:
	explicit MeshCache(Instance &instance);
	void cleanup();

	MeshId         add_mesh(Instance &instance, CPUMesh &mesh);
	const GPUMesh &get_gpu_mesh(MeshId id);

  private:
	std::vector<GPUMesh> _meshes;

	void upload_mesh(Instance &instance, const CPUMesh &cpuMeshh, GPUMesh &gpuMesh) const;
};

struct SkinnedMesh
{
	Buffer skinnedVertexBuffer;
};

// ---------------------------------------------------------------------------------------------
// gfx::Skeleton
// ---------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------
// gfx::Scene
// ---------------------------------------------------------------------------------------------
struct SceneNode
{
	std::string     name;             // glTF node name
	core::Transform transform;        // local transform (T * R * S)

	int meshIndex = -1;        // index into Scene::meshes
	int skinId    = -1;        // used for skeletal animation
	int lightId   = -1;        // if the node is a light
	int cameraId  = -1;        // if the node is a camera

	SceneNode                              *parent = nullptr;
	std::vector<std::unique_ptr<SceneNode>> children;        // recursive scene graph
};

struct SceneMesh
{
	std::vector<MeshId>     primitives;                // e.g. index into vertex/index buffer
	std::vector<MaterialId> primitiveMaterials;        // one material per primitive
};

struct Scene
{
	std::filesystem::path path;

	std::vector<std::unique_ptr<SceneNode>> nodes;

	std::vector<SceneMesh> meshes;
	std::vector<SceneMesh> skeletons;
};

// ---------------------------------------------------------------------------------------------
// gfx::Sprite
// ---------------------------------------------------------------------------------------------
#pragma region gfx::Sprite
struct WVK_API Sprite
{
  public:
	Sprite() = default;
	explicit Sprite(const std::shared_ptr<Texture> &texture);

	std::shared_ptr<Texture> texture;
	uint32_t                 texture_id = 0;
	glm::vec2                texture_size;

	glm::vec2 uv0 = glm::vec2{0.f};
	glm::vec2 uv1 = glm::vec2{1.f};

	glm::vec2 pivot = glm::vec2{0.f};
	glm::vec4 color = glm::vec4{1.f, 1.f, 1.f, 1.f};

	void set_texture(const std::shared_ptr<Texture> &texture);
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
	void init(Instance &instance, VkFormat drawImageFormat, std::string_view name = "");
	void cleanup(Instance &instance);

	void begin_draw();
	void end_draw();

	void draw(VkCommandBuffer cmd, Instance &instance, const std::shared_ptr<Texture> &drawImage);
	void draw(VkCommandBuffer cmd, Instance &instance, const std::shared_ptr<Texture> &drawImage, const glm::mat4 viewProjection);

	void draw_sprite(const Sprite &sprite, const glm::vec2 &position, float rotation, const glm::vec2 &scale, uint32_t shaderId = SPRITE_SHADER_ID);

	void draw_sprite(const Sprite &sprite, const glm::mat4 &transform, uint32_t shaderId = SPRITE_SHADER_ID);

	void draw_text();        // TODO

	void draw_rectangle(Instance &instance, int posX, int posY, int width, int height, const glm::vec4 &color);
	void draw_rectangle_v(Instance &instance, const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color);
	void draw_rectangle_rec(Instance &instance, const core::Rectangle &rec, const glm::vec4 &color);
	void draw_rectangle_pro(Instance &instance, const core::Rectangle &rec, const glm::vec2 &origin, float rotation, const glm::vec4 &color);

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
class Texture;

class WVK_API TextureCache
{
  public:
	explicit TextureCache(Instance &instance);
	void cleanup();

	TextureId      add_texture(Instance &instance, Texture material);
	const Texture &get_texture(TextureId id);

	TextureId get_free_texture_id() const;

  private:
	std::vector<Texture> textures;
};

// ---------------------------------------------------------------------------------------------
// gfx::ImGuiRenderer
// ---------------------------------------------------------------------------------------------
class WVK_API ImGuiRenderer
{
  public:
	explicit ImGuiRenderer(Instance &instance, GLFWwindow *window, VkFormat drawImageFormat, std::string_view name = "");

	void cleanup(VkDevice device);

	void begin_draw();
	void end_draw();
	void draw(VkCommandBuffer cmd, Instance &instance, VkImageView view, VkExtent2D extent);

	ImGuiContext *get_context() { return _context; }

  private:
	bool _is_initialized = false;

	ImGuiContext *_context;

	void _init(Instance &instance, GLFWwindow *window, VkFormat drawImageFormat, std::string_view name = "");

	ImGuiRenderingPipeline _drawing_pipeline;
};

// ---------------------------------------------------------------------------------------------
// gfx::GameRenderer
// ---------------------------------------------------------------------------------------------
}        // namespace wvk::gfx
