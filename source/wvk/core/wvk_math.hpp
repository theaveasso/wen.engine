#pragma once

#include <wvk/common/wvk_defines.hpp>

#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace wvk::core
{
/*
        ^ +Y
        |
        |
        |
<----(^._.^)----->
-X     /         +X
      /
     v  +Z
*/
inline constexpr glm::vec3 global_up_axis{0.f, 1.f, 0.f};
inline constexpr glm::vec3 global_down_axis{0.f, -1.f, 0.f};

inline constexpr glm::vec3 global_front_axis{0.f, 0.f, 1.f};
inline constexpr glm::vec3 global_back_axis{0.f, 0.f, -1.f};

inline constexpr glm::vec3 global_left_axis{1.f, 0.f, 0.f};
inline constexpr glm::vec3 global_right_axis{-1.f, 0.f, 0.f};

class WVK_API Transform
{
  public:
	Transform() = default;
	Transform(const glm::mat4 &m);

	inline bool operator==(const Transform &rhs) const { return _position == rhs._position && _heading == rhs._heading && _scale == rhs._scale; }

	const glm::mat4 &as_matrix() const;

	Transform operator*(const Transform &rhs) const;
	Transform inverse() const;

	bool is_identity() const;

	inline glm::vec3 get_local_up() const { return _heading * global_up_axis; }
	inline glm::vec3 get_local_front() const { return _heading * global_front_axis; }
	inline glm::vec3 get_local_right() const { return _heading * global_right_axis; }

	inline void set_position(const glm::vec3 &p);
	inline void set_heading(const glm::quat &h);
	inline void set_scale(const glm::vec3 &s);

	inline const glm::vec3 &get_position() const { return _position; }
	inline const glm::quat &get_heading() const { return _heading; }
	inline const glm::vec3 &get_scale() const { return _scale; }

  private:
	glm::vec3 _position = glm::vec3{};
	glm::quat _heading  = glm::identity<glm::quat>();
	glm::vec3 _scale    = glm::vec3{1.f};

	mutable glm::mat4 _transform_matrix = glm::mat4{1.0f};
	mutable bool      _is_dirty         = false;
};

struct WVK_API Rectangle
{
	Rectangle() = default;        // zero-initialized
	Rectangle(float x, float y, float with, float height);

	Rectangle(const Rectangle &r);

	Rectangle(const glm::vec2 &position, const glm::vec2 &size);

	bool contains(const glm::vec2 &point) const;

	bool intersects(const Rectangle &rect) const;

	glm::vec2 get_position() const { return glm::vec2{x, y}; }
	void      set_position(const glm::vec2 &position);

	glm::vec2 get_size() const { return glm::vec2{width, height}; }
	void      set_size(const glm::vec2 &size);

	glm::vec2 get_top_left() const { return glm::vec2{x, y}; }
	glm::vec2 get_top_right() const { return glm::vec2{x + width, y}; }
	glm::vec2 get_bottom_left() const { return glm::vec2{x, y + height}; }
	glm::vec2 get_bottom_right() const { return glm::vec2{x + width, y + height}; }
	glm::vec2 get_center() const { return glm::vec2{(x + width) / 2, (y + height) / 2}; }

	void move(const glm::vec2 &offset);

	// data members
	float x, y, width, height;
};

// equality operator
bool operator==(const Rectangle &recA, const Rectangle &recB);

// inequality operator
bool operator!=(const Rectangle &recA, const Rectangle &recB);

glm::vec2 get_intersection_depth(const Rectangle &recA, const Rectangle &recB);

// if rect has negative width or height, left-top corner is changed
// and w, h become positive in returned rect
Rectangle get_absolute_rect(const Rectangle &rect);
}        // namespace wvk::core