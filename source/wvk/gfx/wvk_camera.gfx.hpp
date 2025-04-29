#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "wvk/common/wvk_defines.hpp"
#include "wvk/core/wvk_math.hpp"

namespace wvk
{
#define CAMERA_MOVE_SPEED 5.4f
#define CAMERA_ROTATION_SPEED 0.03f
#define CAMERA_PAN_SPEED 0.2f

#define CAMERA_MOUSE_MOVE_SENSITIVITY 0.003f
#define CAMERA_ORBITAL_SPEED 0.5f
}        // namespace wvk

namespace wvk::gfx
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

struct Camera
{
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 target   = core::global_back_axis;
	glm::vec3 up       = core::global_up_axis;

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
    Camera          *camera,
    const glm::vec2 &viewSize,
    float            near,
    float            far);

/**
 * Set perspective projection.
 */
WVK_API void camera_init_perspective(
    Camera *camera,
    float   fovy,
    float   aspect,
    float   near,
    float   far);

/**
 * Get the current view matrix.
 */
WVK_API const glm::mat4 &camera_get_view(
    Camera *camera);

/**
 * Get the current projection matrix.
 */
WVK_API const glm::mat4 &camera_get_proj(
    Camera *camera);

/**
 * Recalculate the view matrix from position, target, up.
 */
WVK_API void camera_update_view(
    Camera *camera);

/**
 * Recalculate the projection matrix from fovy, aspect, near, far, projection mode.
 */
void camera_update_proj(
    Camera *camera);

/**
 * Move camera position and target by a world-space offset.
 */
void camera_move(
    Camera          *camera,
    const glm::vec3 &delta);

/**
 * Rotate the camera (free look) around its own axes.
 */
void camera_rotate(
    Camera *camera,
    float   yaw,
    float   pitch);

/**
 * Adjust the camera's field of view for zooming.
 */
void camera_zoom(
    Camera *camera,
    float   deltaFov);
}        // namespace wvk::gfx
