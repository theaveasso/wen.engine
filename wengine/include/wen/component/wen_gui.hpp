#ifndef WEN_COMPONENT_WEN_GUI_HPP_
#define WEN_COMPONENT_WEN_GUI_HPP_

#include <flecs.h>

#include "wen/wen_defines.hpp"

namespace wen::component {
using UpdateCallback = void (*)(flecs::iter& it);

struct WEN_API_EXPORT GuiContext {
  void* ctx;
};

struct WEN_API_EXPORT GuiUpdateCallback {
  UpdateCallback on_update_cb;
};

class GuiComponent {
public:
  explicit GuiComponent(flecs::world& world);
};
} // namespace wen::component

#endif // WEN_COMPONENT_WEN_IMGUI_HPP_
