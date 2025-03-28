#include "wen/component/wen_renderer.hpp"

namespace wen::component {
RendererComponent::RendererComponent(flecs::world& world) {
  world.module<RendererComponent>();

  world.component<Renderer>();
  world.component<RenderPassData>();

  world.component<ColorRGB>();
  world.component<ColorRGBA>();
};
} // namespace wen::component
