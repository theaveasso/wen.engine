#ifndef WEN_COMPONENT_WEN_MATH_HPP_
#define WEN_COMPONENT_WEN_MATH_HPP_

#include <flecs.h>
#include <glm/glm.hpp>

namespace wen::component {

struct Vec2d : public glm::vec2 {};

struct Vec3d : public glm::vec2 {};

class MathComponent {
public:
  explicit MathComponent(flecs::world& world);
};
} // namespace wen::component

#endif // WEN_COMPONENT_WEN_MATH_HPP_
