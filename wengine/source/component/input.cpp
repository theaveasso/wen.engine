#include "wen/component/input.hpp"

namespace wen::component {
InputComponent::InputComponent(flecs::world& world) {
  world.module<Input>();

  world.add<Input>();
}
} // namespace wen::component
