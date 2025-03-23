#ifndef WEN_COMPONENT_RENDERER_HPP_
#define WEN_COMPONENT_RENDERER_HPP_

#include <SDL3/SDL.h>
#include <flecs.h>

#include "wen/wen_defines.hpp"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

namespace wen::component {

enum struct RendererType {
  SDLGPU3,
  SDL3,
};

struct WEN_API_EXPORT SDL_RendererComponent {
  SDL_Renderer*  renderer;
  SDL_GPUDevice* gpu_device;
  RendererType   renderer_type;

  SDL_RendererComponent()
      : renderer(nullptr), gpu_device(nullptr),
        renderer_type(RendererType::SDL3){};
};

class WEN_API_EXPORT SDL_Renderer {
public:
  explicit SDL_Renderer(flecs::world& world);
};
} // namespace wen::component

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // WEN_COMPONENT_RENDERER_HPP_
