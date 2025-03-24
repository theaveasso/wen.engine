#ifndef WEN_COMPONENT_TRANSFORM_HPP_
#define WEN_COMPONENT_TRANSFORM_HPP_

#include <flecs.h>
#include <glm/glm.hpp>

#include "wen/wen_defines.hpp"

namespace wen::component {
namespace transform {
struct WEN_API_EXPORT Vec2 : public glm::vec2 {
};

struct WEN_API_EXPORT Transform2d {
  glm::vec2 position = {0.0f, 0.0f};
  glm::vec2 scale    = {1.0, 1.0};
  float     angle    = 0.0;

  Transform2d() = default;
};
} // namespace transform

class WEN_API_EXPORT Transform {
public:
  explicit Transform(flecs::world& world);
};
} // namespace wen::component

#endif // WEN_COMPONENT_TRANSFORM_HPP_