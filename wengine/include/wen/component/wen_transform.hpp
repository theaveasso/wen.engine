#ifndef WEN_COMPONENT_WEN_TRANSFORM_HPP_
#define WEN_COMPONENT_WEN_TRANSFORM_HPP_

#include "wen/component/wen_math.hpp"
#include <flecs.h>
#include <glm/glm.hpp>

namespace wen::component {

struct Transform2d {
  Vec2d position;
  Vec2d scale;
  float angle;
};

using Position2d = Vec2d;


class TransformComponent {
public:
  explicit TransformComponent(flecs::world& world);
};
} // namespace wen::component

#endif // WEN_COMPONENT_WEN_TRANSFORM_HPP_
