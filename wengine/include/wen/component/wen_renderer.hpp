#ifndef WEN_COMPONENT_WEN_RENDERER_HPP_
#define WEN_COMPONENT_WEN_RENDERER_HPP_

#include <SDL3/SDL.h>
#include <flecs.h>

namespace wen::component {
struct Renderer {
  SDL_Renderer*  renderer;
  SDL_GPUDevice* gpu;
};

class RendererComponent {
public:
  explicit RendererComponent(flecs::world& world);
};
} // namespace wen::component

#endif // WEN_COMPONENT_WEN_RENDERER_HPP_
