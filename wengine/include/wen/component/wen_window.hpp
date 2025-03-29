#ifndef WEN_COMPONENT_WEN_WINDOW_HPP_
#define WEN_COMPONENT_WEN_WINDOW_HPP_

#include <flecs.h>
#include <SDL3/SDL.h>

#include "wen/internal/wen_internal_defines.hpp"

namespace wen::component {
struct WEN_API WindowConfig {
  int width;
  int height;

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
