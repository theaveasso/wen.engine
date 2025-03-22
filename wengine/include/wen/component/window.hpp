#ifndef WEN_COMPONENT_WINDOW_HPP_
#define WEN_COMPONENT_WINDOW_HPP_

#include <SDL3/SDL.h>
#include <flecs.h>

#include "wen/wen_defines.hpp"

namespace wen::component {
struct WEN_API_EXPORT WindowComponent {
  SDL_Window* window = nullptr;
  int32_t     width  = 1280;
  int32_t     height = 720;

  WindowComponent() = default;
  WindowComponent(int32_t w, int32_t h) : width(w), height(h) {};
};

class WEN_API_EXPORT Window {
public:
  explicit Window(flecs::world& world);
};
} // namespace wen::component


#endif // WEN_COMPONENT_WINDOW_HPP_
