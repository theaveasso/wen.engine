#include "wen/component/gui.hpp"

namespace wen::component {
WindowConfig::WindowConfig(int width_, int height_, std::string_view title_)
    : width(width_), height(height_), title(title_) {
}

GUI::GUI() : GUI(1280, 720, "WEN") {
}

GUI::GUI(int width_, int height_, std::string_view title_)
    : config(WindowConfig(width_, height_, title_)), window(), renderer() {
}
GUIComponent::GUIComponent(flecs::world& world) {
  world.module<GUIComponent>();

  world.component<GUI>("WEN::GUI");
}

} // namespace wen::component
