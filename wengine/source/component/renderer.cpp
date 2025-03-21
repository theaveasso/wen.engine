#include "wen/component/renderer.hpp"

namespace wen::component {
SDL_Renderer::SDL_Renderer(flecs::world& world) {
  world.module<SDL_Renderer>();

  world.component<SDL_RendererComponent>();
}
} // namespace wen::component
