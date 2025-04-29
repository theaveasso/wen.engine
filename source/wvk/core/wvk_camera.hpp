#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "wvk/common/wvk_defines.hpp"
#include "wvk_math.hpp"

namespace wvk
{
#define CAMERA_MOVE_SPEED 5.4f
#define CAMERA_ROTATION_SPEED 0.03f
#define CAMERA_PAN_SPEED 0.2f

#define CAMERA_MOUSE_MOVE_SENSITIVITY 0.003f
#define CAMERA_ORBITAL_SPEED 0.5f
}        // namespace wvk

namespace wvk::core
{

enum class CameraProjection
{
	PERSPECTIVE,
	ORTHOGRAPHIC
};

enum class CameraMode
{
	FREE,
	ORBITAL,
	FIRST_PERSON,
	THIRD_PERSON
};

struct Camera2d
{
	glm::vec2 offset;
	glm::vec2 target;
	float     rotation;
	float     zoom;

	glm::mat4 view = glm::mat4{1.0f};
};

const glm::mat4 &camera2d_get_view(const Camera2d &camera);

struct Camera3d
{
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 target   = global_back_axis;
	glm::vec3 up       = global_up_axis;

	glm::mat4 view = glm::mat4{1.0f};
	glm::mat4 proj = glm::mat4{1.0f};

	// perspective params
	float fovy     = 45.0f;
	float aspect   = 1.778f;
	float nearClip = 0.1f;
	float farClip  = 100.f;

	// ortho params
	float     xScale;
	float     yScale;
	glm::vec2 viewSize;

	CameraProjection projection = CameraProjection::PERSPECTIVE;
};

/**
 * Set orthographic projection.
 */
WVK_API void camera_init_orthographic(
    Camera3d        *camera,
    const glm::vec2 &viewSize,
    float            near,
    float            far);

/**
 * Set perspective projection.
 */
WVK_API void camera_init_perspective(
    Camera3d *camera,
    float     fovy,
    float     aspect,
    float     near,
    float     far);

/**
 * Get the current view matrix.
 */
WVK_API const glm::mat4 &camera3d_get_view(
    Camera3d *camera);

/**
 * Get the current projection matrix.
 */
WVK_API const glm::mat4 &camera3d_get_proj(
    Camera3d *camera);

/**
 * Recalculate the view matrix from position, target, up.
 */
WVK_API void camera_update_view(
    Camera3d *camera);

/**
 * Recalculate the projection matrix from fovy, aspect, near, far, projection mode.
 */
void camera_update_proj(
    Camera3d *camera);

/**
 * Move camera position and target by a world-space offset.
 */
void camera_move(
    Camera3d        *camera,
    const glm::vec3 &delta);

/**
 * Rotate the camera (free look) around its own axes.
 */
void camera_rotate(
    Camera3d *camera,
    float     yaw,
    float     pitch);

/**
 * Adjust the camera's field of view for zooming.
 */
void camera_zoom(
    Camera3d *camera,
    float     deltaFov);
}        // namespace wvk::core