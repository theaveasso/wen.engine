#include "wen/component/wen_primitive.hpp"

namespace wen::component {
PrimitiveComponent::PrimitiveComponent(flecs::world& world) {
  world.module<PrimitiveComponent>();

  world.component<Primitive>();
  world.component<PrimitiveQuery>();
}
} // namespace wen::component
