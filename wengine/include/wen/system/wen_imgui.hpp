#ifndef WEN_SYSTEM_WEN_IMGUI_HPP_
#define WEN_SYSTEM_WEN_IMGUI_HPP_

#include <flecs.h>

namespace wen::system {
class ImguiSystem {
public:
  explicit ImguiSystem(flecs::world& world);
};
} // namespace wen::system

#endif // WEN_SYSTEM_WEN_IMGUI_HPP_
