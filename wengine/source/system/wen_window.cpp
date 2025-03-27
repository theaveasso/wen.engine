#include "wen/component/wen_window.hpp"

#include "wen/system/wen_window.hpp"
#include "wen/wen.hpp"

namespace wen::system {
static void InitWindow(flecs::iter& it) {
  while (it.next()) {
    if (it.count() > 1) {
      return;
    }
    auto config = it.field<component::WindowConfig>(0);

    auto sdl_window =
        SDL_CreateWindow("WEN Engine", config->width, config->height, 0);
    if (!sdl_window) {
      return;
    }

    it.world().lookup(WEN_APP).emplace<component::Window>(sdl_window);
  }
}

static void ShutdownWindow(flecs::entity e, component::Window& c) {
  auto window = e.get_mut<component::Window>();
  SDL_DestroyWindow(window->window);
}

WindowSystem::WindowSystem(flecs::world& world) {
  world.module<WindowSystem>();

  world.import <component::WindowComponent>();

  world.observer<component::WindowConfig>().event(flecs::OnSet).run(InitWindow);

  world.observer<component::Window>()
      .event(flecs::OnRemove)
      .each(ShutdownWindow);
}
} // namespace wen::system
