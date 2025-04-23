#include "wvk_core.hpp"

#include <stb_image.h>

namespace wvk::core
{
#pragma region core::ImageData
ImageData::ImageData(std::span<const uint8_t> imageData, bool useFloat)
{
	if (!useFloat)
	{
		data = stbi_load_from_memory(imageData.data(), WVK_CAST(uint32_t, imageData.size()), &width, &height, &channels, STBI_rgb_alpha);
	}
	else
	{
		data = stbi_loadf_from_memory(imageData.data(), WVK_CAST(uint32_t, imageData.size()), &width, &height, &channels, STBI_rgb_alpha);
	}
}
ImageData::ImageData(std::span<const char> imageData, bool useFloat)
{
	if (!useFloat)
	{
		data = stbi_load_from_memory(WVK_RCAST(const uint8_t *, imageData.data()), WVK_CAST(uint32_t, imageData.size()), &width, &height, &channels, STBI_rgb_alpha);
	}
	else
	{
		data = stbi_loadf_from_memory(WVK_RCAST(const uint8_t *, imageData.data()), WVK_CAST(uint32_t, imageData.size()), &width, &height, &channels, STBI_rgb_alpha);
	}
}
ImageData::ImageData(const std::filesystem::path &path)
{
	if (stbi_is_hdr(path.string().c_str()))
	{
		hdr_data = stbi_loadf(path.string().c_str(), &width, &height, &comp, STBI_rgb_alpha);
	}
	else
	{
		data = stbi_load(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
	}
}
ImageData::~ImageData()
{
	stbi_image_free(data);
}
#pragma endregion
void Camera::init(
    float fovX,
    float zFar,
    float zNear,
    float aspectRatio)
{
	float s = aspectRatio;
	float g = s / glm::tan(fovX / 2.0f);

	_fovy         = 2.0f * glm::atan(1.0f / g);
	_fovx         = fovX;
	_zfar         = zFar;
	_znear        = zNear;
	_aspect_ratio = aspectRatio;

	if (_use_inverse_depth)
		_projection = glm::perspective(_fovy, _aspect_ratio, _zfar, _znear);
	else
		_projection = glm::perspective(_fovy, _aspect_ratio, _znear, _zfar);

	if (_clip_space_y_down)
		_projection[1][1] *= -1;

	_is_initialized = true;
}
void Camera::init_ortho(
    float scale,
    float zNear,
    float zFar)
{
	init_ortho(scale, scale, zNear, zFar);
}
void Camera::init_ortho(
    float xScale,
    float yScale,
    float zNear,
    float zFar)
{
	_ortho        = true;
	_zfar         = zFar;
	_znear        = zNear;
	_x_scale      = xScale;
	_y_scale      = yScale;
	_aspect_ratio = _x_scale / _y_scale;

	if (_use_inverse_depth)
		_projection = glm::ortho(-_x_scale, _x_scale, -_y_scale, _y_scale, _zfar, _znear);
	else
		_projection = glm::ortho(-_x_scale, _x_scale, -_y_scale, _y_scale, _znear, _zfar);

	if (_clip_space_y_down)
		_projection[1][1] *= -1;

	_is_initialized = true;
}
void Camera::init_ortho_2d(
    const glm::vec2   &size,
    float              zNear,
    float              zFar,
    Camera::OriginType type)
{
	set_clip_space_y_down(true);
	_ortho_2d     = true;
	_zfar         = zFar;
	_znear        = zNear;
	_aspect_ratio = size.x / size.y;

	switch (type)
	{
		case OriginType::TopLeft:
			_projection = glm::ortho(0.f, size.x, 0.f, size.y, _znear, _zfar);
			break;
		case OriginType::Center:
			_projection = glm::ortho(-size.x / 2.f, size.x / 2.f, size.y / 2.f, -size.y / 2.f, _znear, _zfar);
			break;
		default:
			WVK_ASSERT_MSG(false, "invalid origin type");
	}

	_view_size      = size;
	_is_initialized = true;
}
glm::mat4 Camera::get_view() const
{
	WVK_ASSERT_MSG(_is_initialized, "camera was not initialized!");
	if (_ortho_2d)
	{
		return glm::translate(glm::mat4{1.f}, -_transform.get_position());
	}

	const glm::vec3 up     = _transform.get_local_up();
	const glm::vec3 target = get_position() + _transform.get_local_front();
	return glm::lookAt(get_position(), target, up);
}
glm::mat4 Camera::get_view_proj() const
{
	WVK_ASSERT_MSG(_is_initialized, "camera was not initialized!");
	return _projection * get_view();
}
void Camera::set_yaw_pitch(
    float yaw,
    float pitch)
{
}
void Camera::set_use_inverse_depth(bool b)
{
}
void Camera::set_clip_space_y_down(bool b)
{
}
void Camera::reinit()
{
}
}        // namespace wvk::core
