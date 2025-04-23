#include "wvk_math.hpp"

#include <glm/gtx/matrix_decompose.hpp>

namespace
{
const auto I = glm::mat4{1.0f};
}

namespace wvk::core
{
const glm::mat4 &Transform::as_matrix() const
{
	if (!_is_dirty)
	{
		return _transform_matrix;
	}

	_transform_matrix = glm::translate(I, _position);
	if (_heading != glm::identity<glm::quat>())
	{
		_transform_matrix *= glm::mat4(_heading);
	}
	_transform_matrix = glm::scale(_transform_matrix, _scale);
	_is_dirty         = false;
	return _transform_matrix;
}
void Transform::set_position(const glm::vec3 &p)
{
	_position = p;
	_is_dirty = true;
}
void Transform::set_heading(const glm::quat &h)
{
	_heading  = glm::normalize(h);
	_is_dirty = true;
}
void Transform::set_scale(const glm::vec3 &s)
{
	_scale    = s;
	_is_dirty = true;
}
Rectangle::Rectangle(float x, float y, float with, float height) :
    x(x), y(y), width(with), height(height)
{
}

Rectangle::Rectangle(const Rectangle &r) :
    x(r.x), y(r.y), width(r.width), height(r.height)
{
}
Rectangle::Rectangle(const glm::vec2 &position, const glm::vec2 &size) :
    x(position.x), y(position.y), width(size.x), height(size.y)
{
}
bool Rectangle::contains(const glm::vec2 &point) const
{
	return point.x >= x &&
	       point.y >= y &&
	       point.x <= x + width &&
	       point.y <= y + height;
}
bool Rectangle::intersects(const Rectangle &rec) const
{
	return !(
	    rec.x + rec.x < x ||
	    rec.x > x + width ||
	    rec.y + rec.height < y ||
	    rec.y > y + height);
}
void Rectangle::set_position(const glm::vec2 &position)
{
	x = position.x;
	y = position.y;
}
void Rectangle::set_size(const glm::vec2 &size)
{
	width  = size.x;
	height = size.y;
}
void Rectangle::move(const glm::vec2 &offset)
{
}
}        // namespace wvk::core
