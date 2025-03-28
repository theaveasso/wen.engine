#include "wen/component/wen_gui.hpp"

namespace wen::component {
GuiComponent::GuiComponent(flecs::world& world) {
  world.module<GuiComponent>();

  world.component<GuiContext>();
  world.component<GuiUpdateCallback>();
}
} // namespace wen::component
