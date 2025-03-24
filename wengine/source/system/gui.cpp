#include "wen/system/gui.hpp"

#include <iostream>

#include "wen/component/gui.hpp"

namespace wen::system {
static void GUIInitialize([[maybe_unused]] flecs::entity e,
                          component::GUI&                comp) {
  comp.window.window = SDL_CreateWindow(
      comp.config.title.c_str(), comp.config.width, comp.config.height,
      SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
  assert(comp.window.window && "[failed] SDL_CreateWindow!\n");

  comp.renderer.device = SDL_CreateGPUDevice(
      SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXBC |
          SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL,
      false, nullptr);
  assert(comp.renderer.device && "[failed] SDL_CreateGPUDevice\n");

  auto claim_ok = SDL_ClaimWindowForGPUDevice(comp.renderer.device, comp.window.window);
  assert(claim_ok && "[failed] SDL_ClaimWindowForGPUDevice\n");
}

static void GUIShutdown([[maybe_unused]] flecs::entity e,
                        component::GUI&                comp) {
  std::cout << "gui shutdown" << std::endl;
}

GUISystem::GUISystem(flecs::world& world) {
  world.module<GUISystem>();

  world.import <component::GUIComponent>();

  world.observer<component::GUI>().event(flecs::OnSet).each(GUIInitialize);

  world.observer<component::GUI>().event(flecs::OnDelete).each(GUIShutdown);
}
} // namespace wen::system