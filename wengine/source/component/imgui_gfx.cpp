#include "wen/component/imgui_gfx.hpp"

#include <iostream>

namespace wen::component {
ImGui_GFX::ImGui_GFX(flecs::world& world) {
  world.module<ImGui_GFX>();

  world.component<ImGui_GFXComponent>();
}
}