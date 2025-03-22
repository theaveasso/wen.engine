#ifndef WEN_COMPONENT_RENDERER_HPP_
#define WEN_COMPONENT_RENDERER_HPP_

#include <SDL3/SDL.h>
#include <flecs.h>

#include "wen/wen_defines.hpp"

namespace wen::component {
struct WEN_API_EXPORT SDL_RendererComponent {
  SDL_Renderer* renderer = nullptr;

  SDL_RendererComponent() = default;
};

class WEN_API_EXPORT SDL_Renderer {
public:
  explicit SDL_Renderer(flecs::world& world);
};
} // namespace wen::component


#endif // WEN_COMPONENT_RENDERER_HPP_
