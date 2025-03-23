#include "wen/system/window.hpp"

#include "wen/component/gui.hpp"
#include "wen/component/window.hpp"

namespace wen::system {
using namespace component;

static void CreateWindow([[maybe_unused]] flecs::entity e, GUIComponent& gui,
                         WindowComponent& window) {
  auto wnd =
      SDL_CreateWindow(gui.title.c_str(), gui.width, gui.height,
                       SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
  if (wnd == nullptr) {
    ecs_err("failed to create SDL_Window! %s", SDL_GetError());
    return;
  }

  window.window = wnd;
}

static void DestroyWindow([[maybe_unused]] flecs::entity e,
                          WindowComponent&               window_comp) {
  SDL_DestroyWindow(window_comp.window);
}

WindowSystem::WindowSystem(flecs::world& world) {
  world.module<WindowSystem>();

  world.import <GUI>();
  world.import <Window>();

  world.observer<GUIComponent, WindowComponent>()
      .event(flecs::OnSet)
      .each(CreateWindow);

  world.observer<WindowComponent>().event(flecs::OnDelete).each(DestroyWindow);
}
} // namespace wen::system