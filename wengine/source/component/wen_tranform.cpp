#include "wen/component/wen_transform.hpp"

namespace wen::component {
TransformComponent::TransformComponent(flecs::world& world) {
  world.module<TransformComponent>();

  world.component<Transform2d>();
  world.component<Position2d>();
}
} // namespace wen::component
