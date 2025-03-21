#include "wen/component/gui.hpp"

namespace wen::component {
GUI::GUI(flecs::world& world) {
  world.module<GUI>();

  world.component<GUIComponent>();
}
} // namespace wen::component
