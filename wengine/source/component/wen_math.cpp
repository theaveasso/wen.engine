#include "wen/component/wen_math.hpp"

namespace wen::component {
MathComponent::MathComponent(flecs::world& world) {
  world.module<MathComponent>();

  world.component<Vec2d>();

  world.component<Vec3d>();
}
} // namespace wen::component
