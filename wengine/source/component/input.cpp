#include "wen/component/input.hpp"

namespace wen::component {
Input::Input(flecs::world& world) {
  world.module<Input>();

  world.add<InputComponent>();

}
} // namespace wen::component
