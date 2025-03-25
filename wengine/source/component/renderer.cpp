#include "wen/component/renderer.hpp"

namespace wen::component {

GPUShader::GPUShader(const char* filename_, int sampler_count_,
                     int uniform_buffer_count_, int storage_buffer_count_,
                     int storage_texture_count_)
    : sampler_count(sampler_count_),
      uniform_buffer_count(uniform_buffer_count_),
      storage_buffer_count(storage_buffer_count_),
      storage_texture_count(storage_buffer_count_), filename(filename_),
      shader(nullptr) {
}
RendererComponent::RendererComponent(flecs::world& world) {
  world.module<RendererComponent>();

  world.component<GPUShader>();
}
} // namespace wen::component
