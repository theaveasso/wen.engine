#include "wen/component/gui.hpp"

namespace wen::component {
GUIComponent::GUIComponent(flecs::world& world) {
  world.module<GUIComponent>();

  world.component<GUI>();
}
} // namespace wen::component
