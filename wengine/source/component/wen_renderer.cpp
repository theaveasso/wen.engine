#include "wen/component/wen_renderer.hpp"

namespace wen::component {
RendererComponent::RendererComponent(flecs::world& world) {
  world.module<RendererComponent>();

  world.component<Renderer>();
};
} // namespace wen::component
