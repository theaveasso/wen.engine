#ifndef WEN_COMPONENT_RENDERER_HPP_
#define WEN_COMPONENT_RENDERER_HPP_

#include <SDL3/SDL.h>
#include <flecs.h>

namespace wen::component {

extern flecs::entity_t OnBeginFrame;
extern flecs::entity_t OnEndFrame;
extern flecs::entity_t OnDraw;

struct GPUShader {
  int            sampler_count;
  int            uniform_buffer_count;
  int            storage_buffer_count;
  int            storage_texture_count;
  const char*    filename;
  SDL_GPUShader* shader;

  GPUShader(const char* filename_, int sampler_count_,
            int uniform_buffer_count_, int storage_buffer_count_,
            int storage_texture_count_);
};

struct FrameData {
  SDL_GPUGraphicsPipeline* pipeline          = nullptr;
  SDL_GPUBuffer*           vertex_buffer     = nullptr;
  SDL_GPUBuffer*           index_buffer      = nullptr;
  SDL_GPUTexture*          texture           = nullptr;
  SDL_GPUSampler*          sampler           = nullptr;
  SDL_GPUCommandBuffer*    command_buffer    = nullptr;
  SDL_GPUTexture*          swapchain_texture = nullptr;
};

class RendererComponent {
public:
  explicit RendererComponent(flecs::world& world);
};
} // namespace wen::component

#endif // WEN_COMPONENT_RENDERER_HPP_
