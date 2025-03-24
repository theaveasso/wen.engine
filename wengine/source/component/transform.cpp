#include "wen/component/transform.hpp"
#include "wen/system/transform.hpp"

namespace wen::component {
Transform::Transform(flecs::world& world) {
  world.module<wen::component::Transform>();

  world.component<wen::component::transform::Transform2d>();
  world.component<wen::component::transform::Vec2>().member("x", &transform::Vec2::x).member("y", &transform::Vec2::y);
}
} // namespace wen::component