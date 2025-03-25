#include <wen/wen.hpp>

void init_component(flecs::world& world);

int main(int /* argc */, char** /* argv */) {
  flecs::world world;

  world.import <wen::system::Engine>();

  init_component(world);

  world.app().delta_time(1.0 / 60.0).run();

  return 0;
}

void init_component(flecs::world& world) {
  world.component<wen::system::Engine>()
      .emplace<wen::component::Input>()
      .emplace<wen::component::GUI>();
}
