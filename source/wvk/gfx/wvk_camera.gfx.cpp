#include "wvk_camera.gfx.hpp"

#include "wvk/common/wvk_pch.hpp"

namespace wvk::gfx
{

void camera_init_orthographic(
    Camera          *camera,
    const glm::vec2 &viewSize,
    float            near,
    float            far)
{
	camera->projection = CameraProjection::ORTHOGRAPHIC;
	camera->viewSize   = viewSize;
	camera->nearClip   = near;
	camera->farClip    = far;
	camera->xScale     = 1.0f;
	camera->yScale     = 1.0f;

	camera_update_view(camera);
	camera_update_proj(camera);
}

void camera_init_perspective(
    Camera *camera,
    float   fovy,
    float   aspect,
    float   near,
    float   far)
{
	WVK_ASSERT_MSG(std::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f, "");
	camera->projection = CameraProjection::PERSPECTIVE;
	camera->fovy       = fovy;
	camera->aspect     = aspect;
	camera->nearClip   = near;
	camera->farClip    = far;

	camera_update_view(camera);
	camera_update_proj(camera);
}

const glm::mat4 &camera3d_get_view(
    Camera *camera)
{
	return camera->view;
}

const glm::mat4 &camera3d_get_proj(
    Camera *camera)
{
	return camera->proj;
}

void camera_update_view(
    Camera *camera)
{
	camera->view = glm::lookAt(
	    camera->position,
	    camera->target,
	    camera->up);
}

void camera_update_proj(
    Camera *camera)
{
	if (camera->projection == CameraProjection::PERSPECTIVE)
	{
		camera->proj = glm::perspective(
		    glm::radians(camera->fovy),
		    camera->aspect,
		    camera->nearClip,
		    camera->farClip);
	}
	else if (camera->projection == CameraProjection::ORTHOGRAPHIC)
	{
		float halfWidth  = (camera->viewSize.x * 0.5f) * camera->xScale;
		float halfHeight = (camera->viewSize.y * 0.5f) * camera->yScale;
		camera->proj     = glm::ortho(
            -halfWidth,
            halfWidth,
            -halfHeight,
            halfHeight,
            camera->nearClip,
            camera->farClip);
	}
}

void camera_move(
    Camera          *camera,
    const glm::vec3 &delta)
{
	camera->position += delta;
	camera->target += delta;

	camera_update_view(camera);
}

void camera_rotate(
    Camera *camera,
    float   yaw,
    float   pitch)
{
	glm::vec3 direction = glm::normalize(camera->target - camera->position);
	glm::mat4 yawRot    = glm::rotate(glm::mat4(1.0f), glm::radians(yaw), camera->up);
	glm::vec3 right     = glm::normalize(glm::cross(direction, camera->up));
	glm::mat4 pitchRot  = glm::rotate(glm::mat4(1.0f), glm::radians(pitch), right);

	glm::vec3 directionRot = glm::vec3(pitchRot * yawRot * glm::vec4(direction, 0.0f));
	camera->target         = camera->position + directionRot;

	camera_update_view(camera);
}

void camera_zoom(
    Camera *camera,
    float   deltaFov)
{
	camera->fovy = glm::clamp(
	    camera->fovy + deltaFov,
	    1.0f,
	    120.f);

	camera_update_proj(camera);
}
}        // namespace wvk::gfx
