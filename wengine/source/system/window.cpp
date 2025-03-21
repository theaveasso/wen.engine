#include "wen/system/window.hpp"

#include "wen/component/gui.hpp"
#include "wen/component/window.hpp"

namespace wen::system {
using namespace flecs;
using namespace component;

static void CreateWindow(entity e, GUIComponent& gui, WindowComponent& window) {
  auto wnd = SDL_CreateWindow(gui.title.c_str(), gui.width, gui.height,
                              SDL_WINDOW_HIGH_PIXEL_DENSITY);
  if (wnd == nullptr) {
    return;
  }

  window.window = wnd;
};

static void DestroyWindow(entity e, WindowComponent& window_comp) {
  SDL_DestroyWindow(window_comp.window);
};

WindowSystem::WindowSystem(world& world) {
  world.module<WindowSystem>();

  world.import <GUI>();
  world.import <Window>();

  world.observer<GUIComponent, WindowComponent>().event(OnSet).each(
      CreateWindow);

  world.observer<WindowComponent>().event(OnRemove).each(DestroyWindow);
}
} // namespace wen::system
