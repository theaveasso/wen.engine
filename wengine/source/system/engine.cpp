#include "wen/system/engine.hpp"

#include "SDL3/SDL_init.h"
#include "wen/component/gui.hpp"
#include "wen/component/input.hpp"
#include "wen/system/input.hpp"
#include "wen/system/renderer.hpp"
#include "wen/system/window.hpp"

namespace wen {
namespace system {

static void OnDestroy(ecs_world_t* world, void* ctx) {
  SDL_Quit();
};

Engine::Engine(flecs::world& world) {
  world.import <component::GUI>();
  world.import <component::Input>();
  world.import <system::WindowSystem>();
  world.import <system::SDL_RendererSystem>();

  world.system<component::InputComponent>("InputSystem")
      .kind(flecs::PostLoad)
      .each(ProcessEvent);

  world.atfini(OnDestroy);

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    return;
  }
}
} // namespace system
} // namespace wen
