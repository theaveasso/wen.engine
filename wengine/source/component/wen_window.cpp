#include "wen/component/wen_window.hpp"

namespace wen::component {
WindowConfig::WindowConfig(int width_, int height_)
    : width(width_), height(height_) {
}

Window::Window(SDL_Window* window_) : window(window_) {
}

WindowComponent::WindowComponent(flecs::world& world) {
  world.module<WindowComponent>();

  world.component<WindowConfig>()
      .member("width", &WindowConfig::width)
      .member("height", &WindowConfig::height);
  world.component<Window>();
}
} // namespace wen::component
