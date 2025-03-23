#include <wen/wen.hpp>

namespace sandbox {
enum struct TileKind { TURRET, PATH, OTHER };

template <typename T> class Grid {};

using namespace wen::component;
struct Game {
  flecs::entity window;
  flecs::entity level;

  transform::Vec2 center = {};
  float           size   = 0;
};

struct Level {
  Level() = default;
  Level(Grid<TileKind>* arg_map, transform::Vec2 arg_spawn)
      : spawning_point(arg_spawn), map(arg_map) {}

  transform::Vec2 spawning_point = {};
  Grid<TileKind>* map            = nullptr;
};

struct Enemy {};

struct Laser {};

struct Particle {};

namespace prefabs {}
} // namespace sandbox

using namespace wen;

struct game_level {};

void init_game(flecs::world& world);
void init_component(flecs::world& world);

int main(int /* argc */, char** /* argv */) {
  flecs::world world;

  world.import <wen::component::Transform>();
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

  world.component<sandbox::Game>()
      .member("window", &sandbox::Game::window)
      .member("level", &sandbox::Game::level)
      .member("center", &sandbox::Game::center)
      .member("size", &sandbox::Game::size);

  world.component<sandbox::Enemy>();

  world.component<sandbox::Laser>();

  world.component<sandbox::Particle>();
}

void init_game(flecs::world& world) {
  auto& game = world.ensure<sandbox::Game>();
  //  game.center = {};
  //  game.size =

  world.script().filename("").run();
}
