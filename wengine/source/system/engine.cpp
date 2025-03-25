#include "wen/system/engine.hpp"

#include <iostream>

#include "wen/component/gui.hpp"
#include "wen/component/input.hpp"

#include "wen/system/gui.hpp"
#include "wen/system/imgui_gfx.hpp"
#include "wen/system/input.hpp"

namespace wen::system {
static flecs::query on_input;

static void OnDestroy(ecs_world_t* world, void* ctx) {
  SDL_Quit();
};

Engine::Engine(flecs::world& world) {
  world.module<Engine>();
  world.import <component::InputComponent>();
  world.import <system::GUISystem>();

 world.system<component::Input>("InputSystem")
     .kind(flecs::PostLoad)
     .each(ProcessEvent);

 world.atfini(OnDestroy);

 if (!SDL_Init(SDL_INIT_VIDEO)) {
   return;
 }
} // namespace system
} // namespace wen::system
