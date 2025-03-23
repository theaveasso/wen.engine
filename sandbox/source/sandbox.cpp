#include <wen/wen.hpp>

using namespace wen;

enum struct TileKind { TURRET, PATH, OTHER };

namespace prefabs {}

void init_game(flecs::world& world);
void init_component(flecs::world& world);

int main(int /* argc */, char** /* argv */) {
  flecs::world world;

  world.import <wen::system::Engine>();

  init_component(world);

  world.app().delta_time(1.0 / 60.0).run();

  return 0;
}

void init_component(flecs::world& world) {
  world.component<system::Engine>()
      .emplace<component::GUIComponent>()
      .emplace<component::InputComponent>()
      .emplace<component::WindowComponent>(600, 800)
      .emplace<component::SDL_RendererComponent>()
      .emplace<component::ImGui_GFXComponent>();
}

void init_game(flecs::world& world) {
}
