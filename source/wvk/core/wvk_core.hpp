#pragma once

#include "wvk/common/wvk_pch.hpp"
#include "wvk/common/wvk_defines.hpp"

#include <glm/geometric.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/type_aligned.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <stb_image.h>

#include "wvk/core/wvk_math.hpp"
#include "wvk/gfx/wvk_gfxtypes.hpp"

namespace wvk::gfx
{
class Instance;
}        // namespace wvk::gfx

namespace wvk::core
{
/**
 * @brief
 */
/// ----------------------------------------------------------------------------------------
/// Image
/// ----------------------------------------------------------------------------------------
struct WVK_API ImageData
{
	WVK_MOVABLE_ONLY(ImageData);

	explicit ImageData(std::span<const uint8_t> imageData, bool useFloat = false);
	explicit ImageData(std::span<const char> imageData, bool useFloat = false);
	explicit ImageData(const std::filesystem::path &path);
	~ImageData();

	// data
	void *data     = nullptr;
	int   width    = 0;
	int   height   = 0;
	int   channels = 0;

	// HDR only
	float *hdr_data = nullptr;
	bool   is_hdr   = false;
	int    comp     = 0;
};

struct IndirectDrawDataAndMeshData
{
	uint32_t indexCount;
	uint32_t firstIndex;
	uint32_t vertexOffset;
	uint32_t instanceCount;
	uint32_t firstInstance;
	uint32_t meshId;
	int      materialId;
};

struct Model
{
	//	std::vector<Mesh>                        meshes;
	//	std::vector<Material>                    materials;
	std::vector<std::unique_ptr<ImageData>>  textures;
	std::vector<IndirectDrawDataAndMeshData> indirectDrawDataSet;

	uint32_t totalVertexSize = 0;
	uint32_t totalIndexSize  = 0;
	uint32_t indexCount      = 0;
};
#pragma endregion

#pragma region core::Camera
class WVK_API  Camera final
{
  public:
	enum class OriginType
	{
		TopLeft,
		Center
	};

  public:
	void init(float fovX, float zFar, float zNear, float aspectRatio);

	void init_ortho(float scale, float zNear, float zFar);
	void init_ortho(float xScale, float yScale, float zNear, float zFar);

	void init_ortho_2d(const glm::vec2 &size, float zNear = 0.f, float zFar = 1.0f, OriginType type = OriginType::TopLeft);

	glm::mat4 get_view() const;
	glm::mat4 get_view_proj() const;

	void set_yaw_pitch(float yaw, float pitch);

	[[nodiscard]] inline const Transform &get_transform() const { return _transform; }

	inline void      set_position(const glm::vec3 &p) { _transform.set_position(p); }
	inline glm::vec3 get_position() const { return _transform.get_position(); }

	inline void      set_position_2d(const glm::vec2 &p) { _transform.set_position({p, 1.0f}); }
	inline glm::vec2 get_position_2d() const { return glm::vec2{_transform.get_position()}; }

	inline void             set_heading(const glm::quat &h) { _transform.set_heading(h); }
	inline const glm::quat &get_heading() const { return _transform.get_heading(); }

	inline void             set_projection(const glm::mat4 &proj) { _projection = proj; }
	inline const glm::mat4 &get_projection() const { return _projection; }

	void        set_use_inverse_depth(bool b);
	inline bool uses_inverse_depth() const { return _use_inverse_depth; }

	void        set_clip_space_y_down(bool b);
	inline bool is_clip_space_y_down() const { return _clip_space_y_down; }

	inline bool is_ortho() const { return _ortho; }

	inline float get_znear() const { return _znear; }
	inline float get_zfar() const { return _zfar; }
	inline float get_aspect_ratio() const { return _aspect_ratio; }
	inline float get_fovx() const { return _fovx; }
	inline float get_fovy() const { return _fovy; }

  private:
	void reinit();

	bool _is_initialized = false;

	Transform _transform;

	glm::mat4 _projection;
	bool      _ortho    = false;
	bool      _ortho_2d = false;

	bool _use_inverse_depth = false;
	bool _clip_space_y_down = true;

	float _znear        = 1.f;
	float _zfar         = 75.f;
	float _aspect_ratio = 16.f / 9.f;
	float _fovx         = glm::radians(90.f);
	float _fovy         = glm::radians(60.f);

	float     _x_scale;
	float     _y_scale;
	glm::vec2 _view_size;
};
#pragma endregion
}        // namespace wvk::core