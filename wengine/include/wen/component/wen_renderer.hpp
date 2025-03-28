#ifndef WEN_COMPONENT_WEN_RENDERER_HPP_
#define WEN_COMPONENT_WEN_RENDERER_HPP_

#include <SDL3/SDL.h>
#include <flecs.h>

namespace wen::component {
struct ColorRGB {
  float r, g, b;
};

struct ColorRGBA {
  float r, g, b, a;
};

struct PositionTextureVertex {
  float x, y, z;
  float u, v;
};

struct Renderer {
  SDL_Renderer*  renderer;
  SDL_GPUDevice* gpu;
};

struct RenderPassData {
  SDL_GPUBuffer*
      quad_vertex_buffer; // TODO: create custom struct with destructor release
  SDL_GPUBuffer* quad_indices_buffer; // TODO: --
  // TODO: SDL_GPUBuffer* quad_normal_buffer;
  // TODO: SDL_GPUSampler* quad_samplers[SDL_arraysize(SamplersName)];

  SDL_GPUGraphicsPipeline* pipeline;
  SDL_GPUColorTargetInfo   color_target_info;
  SDL_GPUCommandBuffer*    command_buffer;
  SDL_GPUTexture*          swapchain_texture;
};

class RendererComponent {
public:
  explicit RendererComponent(flecs::world& world);
};
} // namespace wen::component

#endif // WEN_COMPONENT_WEN_RENDERER_HPP_
