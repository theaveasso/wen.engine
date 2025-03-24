#include "wen/system/engine.hpp"

#include <iostream>

#include "wen/component/gui.hpp"
#include "wen/component/input.hpp"
#include "wen/component/renderer.hpp"

#include "wen/system/imgui_gfx.hpp"
#include "wen/system/input.hpp"
#include "wen/system/renderer.hpp"
#include "wen/system/window.hpp"
#include "wen/system/gui.hpp"


namespace wen::system {
static void OnDestroy(ecs_world_t* world, void* ctx) {
  ////// TODO: Stop everything here
  SDL_Quit();
  std::cout << "wen engine shutdown ..." << std::endl;
};

Engine::Engine(flecs::world& world) {
  world.import <component::Input>();

  world.import <system::GUISystem>();

  world.system<component::InputComponent>("InputSystem")
      .kind(flecs::PostLoad)
      .each(ProcessEvent);

  world.atfini(OnDestroy);

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    return;
  }
} // namespace system
} // namespace wen::system
