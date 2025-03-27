#ifndef WEN_COMPONENT_WEN_WINDOW_HPP_
#define WEN_COMPONENT_WEN_WINDOW_HPP_

#include "wen/wen_defines.hpp"

#include <flecs.h>

namespace wen::component {
struct WEN_API_EXPORT WindowConfig {
  int         width;
  int         height;

  explicit WindowConfig(int width_, int height_);
};

struct Window {
  SDL_Window* window;

  explicit Window(SDL_Window* window_);
};

class WindowComponent {
public:
  explicit WindowComponent(flecs::world& world);
};
} // namespace wen::component

#endif // WEN_COMPONENT_WEN_WINDOW_HPP_
