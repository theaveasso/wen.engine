#ifndef WEN_COMPONENT_GUI_HPP_
#define WEN_COMPONENT_GUI_HPP_

#include "wen/wen_defines.hpp"

#include <flecs.h>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

namespace wen::component {
struct WindowConfig {
  int         width;
  int         height;
  std::string title;

  WindowConfig(int width_, int height_, std::string_view title_);
};

struct Window {
  SDL_Window* window;
};

struct Renderer {
  SDL_GPUDevice* device;
};

struct WEN_API_EXPORT GUI {
  WindowConfig config;
  Window       window;
  Renderer     renderer;

  GUI();
  GUI(int width_, int height_, std::string_view title_);
};

class GUIComponent {
public:
  explicit GUIComponent(flecs::world& world);
};
} // namespace wen::component

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // WEN_COMPONENT_GUI_HPP_
