#ifndef WEN_COMPONENT_GUI_HPP_
#define WEN_COMPONENT_GUI_HPP_

#include "wen/wen_defines.hpp"

#include <flecs.h>
#include <string>

namespace wen::component {
struct WindowConfig {
  int         width  = 1280;
  int         height = 720;
  std::string title  = "WEN";
};

struct Window {
  SDL_Window* window = nullptr;
};

struct Renderer {
  SDL_GPUDevice* device = nullptr;
};

struct WEN_API_EXPORT GUI {
  WindowConfig config;
  Window       window;
  Renderer     renderer;

  GUI() = default;
};

class GUIComponent {
public:
  explicit GUIComponent(flecs::world& world);
};
} // namespace wen::component

#endif // WEN_COMPONENT_GUI_HPP_
