#include <iostream>

#include "wen/system/renderer.hpp"

#include "wen/component/renderer.hpp"
#include "wen/component/window.hpp"

namespace wen::system {
using namespace flecs;
using namespace wen::component;

internal void CreateSDLRenderer(entity e, WindowComponent& window_comp,
                                SDL_RendererComponent& renderer_comp) {
  auto renderer = SDL_CreateRenderer(window_comp.window, 0);
  if (renderer == nullptr) {
    std::cout << "failed CreateSDLRenderer" << std::endl;
    return;
  }

  renderer_comp.renderer = renderer;
}

internal void DestroySDLRenderer(entity e, SDL_RendererComponent& comp) {

  std::cout << "DestroySDLRenderer" << std::endl;
  SDL_DestroyRenderer(comp.renderer);
}

SDL_RendererSystem::SDL_RendererSystem(world& world) {
  world.module<SDL_RendererSystem>();

  world.import <Window>();
  world.import <SDL_Renderer>();

  world.observer<WindowComponent, SDL_RendererComponent>().event(OnSet).each(
      CreateSDLRenderer);

  world.observer<SDL_RendererComponent>().event(OnRemove).each(
      DestroySDLRenderer);
}

} // namespace wen::system
