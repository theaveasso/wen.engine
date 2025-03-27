#include "wen/component/wen_input.hpp"

namespace wen::component {
InputComponent::InputComponent(flecs::world& world) {
  world.module<InputComponent>();

  world.component<Input>();
}
} // namespace wen::component
