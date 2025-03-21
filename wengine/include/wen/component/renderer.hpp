#ifndef __WEN_COMPONENT_RENDERER_HPP_
#define __WEN_COMPONENT_RENDERER_HPP_

#include <SDL3/SDL.h>
#include <flecs.h>

#include "wen/wen_defines.hpp"

namespace wen {
namespace component {
struct WEN_API_EXPORT SDL_RendererComponent {
  SDL_Renderer* renderer = nullptr;

  SDL_RendererComponent() = default;
};

class WEN_API_EXPORT SDL_Renderer {
public:
  SDL_Renderer(flecs::world& world);
};
} // namespace component
} // namespace wen

#endif // __WEN_COMPONENT_RENDERER_HPP_
