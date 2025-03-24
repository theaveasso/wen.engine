#ifndef WEN_COMPONENT_IMGUI_GFX_HPP_
#define WEN_COMPONENT_IMGUI_GFX_HPP_

#include <SDL3/SDL.h>
#include <flecs.h>
#include <imgui.h>

#include "wen/wen_defines.hpp"

namespace wen::component {
struct WEN_API_EXPORT ImGui_GFXComponent {
  ImGuiIO* io = nullptr;
  ImGuiContext* ctx = nullptr;

  ImGui_GFXComponent() = default;
};

class WEN_API_EXPORT ImGui_GFX {
public:
  explicit ImGui_GFX(flecs::world& world);
};
} // namespace wen::component

#endif // WEN_COMPONENT_IMGUI_GFX_HPP_
