#include "wen/component/wen_imgui.hpp"

namespace wen::component {
ImguiComponent::ImguiComponent(flecs::world& world) {
  world.module<ImguiComponent>();

  world.component<Imgui>();
}
} // namespace wen::component
