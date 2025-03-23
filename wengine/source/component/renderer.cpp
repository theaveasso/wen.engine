#include "wen/component/renderer.hpp"

namespace wen::component {
SDL_Renderer::SDL_Renderer(flecs::world& world) {
  world.module<SDL_Renderer>();

  world.component<SDL_RendererComponent>().member(
      "render_type", &SDL_RendererComponent::renderer_type);
}
} // namespace wen::component
