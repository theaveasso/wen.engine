#ifndef WEN_COMPONENT_WEN_IMGUI_HPP_
#define WEN_COMPONENT_WEN_IMGUI_HPP_

#include <SDL3/SDL.h>
#include <flecs.h>
#include <imgui.h>

#include "wen/wen_defines.hpp"

namespace wen::component {
struct WEN_API_EXPORT Imgui {
  ImGuiIO*      io;
  ImGuiContext* ctx = nullptr;
};

class ImguiComponent {
public:
  explicit ImguiComponent(flecs::world& world);
};
} // namespace wen::component

#endif // WEN_COMPONENT_WEN_IMGUI_HPP_
