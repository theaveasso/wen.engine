#ifndef WEN_COMPONENT_RENDERER_HPP_
#define WEN_COMPONENT_RENDERER_HPP_

#include "wen/wen_defines.hpp"

#include <flecs.h>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

namespace wen::component {

flecs::entity_t OnBeginFrame;
flecs::entity_t OnEndFrame;
flecs::entity_t OnDraw;

struct OnDraw {
  std::byte _;
};

struct OnEndFrame {
  std::byte _;
};

struct GPUShader {
  Uint32         sampler_count;
  Uint32         uniform_buffer_count;
  Uint32         storage_buffer_count;
  Uint32         storage_texture_count;
  std::string    filename;
  SDL_GPUShader* shader;
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

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // WEN_COMPONENT_RENDERER_HPP_
