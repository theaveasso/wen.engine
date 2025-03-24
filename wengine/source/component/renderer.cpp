#include "wen/component/renderer.hpp"

namespace wen::component {
RendererComponent::RendererComponent(flecs::world& world) {
  world.module<RendererComponent>();

  world.component<GPUShader>();
}
} // namespace wen::component