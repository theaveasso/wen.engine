#include "wen/component/input.hpp"

namespace wen::component {
InputComponent::InputComponent(flecs::world& world) {
  world.module<InputComponent>();

  world.component<Input>();
  world.component<InputMapping>();
}
} // namespace wen::component
