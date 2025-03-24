#ifndef WEN_SYSTEM_IMGUI_HPP_
#define WEN_SYSTEM_IMGUI_HPP_

#include <flecs.h>

#include "wen/wen_defines.hpp"

namespace wen::system {
class WEN_API_EXPORT ImGui_GFXSystem {
public:
  explicit ImGui_GFXSystem(flecs::world& world);
};
} // namespace wen::system


#endif // WEN_SYSTEM_IMGUI_HPP_
