#include "wen/system/gui.hpp"

#include "wen/component/gui.hpp"

namespace wen::system {
static void GUIInitialize([[maybe_unused]] flecs::entity e,
                          component::GUI&                comp) {
  comp.window.window = SDL_CreateWindow(
      comp.config.title.c_str(), comp.config.width, comp.config.height,
      SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
  if (!comp.window.window) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "failed to create SDL window %s",
                 SDL_GetError());
    assert(comp.window.window && "[failed] SDL_CreateWindow!\n");
    return;
  }

  comp.renderer.device = SDL_CreateGPUDevice(
      SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXBC |
          SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL,
      false, nullptr);
  if (!comp.renderer.device) {
    assert(comp.renderer.device && "[failed] SDL_CreateGPUDevice\n");
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "failed to create SDL gpu device %s", SDL_GetError());
    return;
  }

  auto claim_ok =
      SDL_ClaimWindowForGPUDevice(comp.renderer.device, comp.window.window);
  if (!claim_ok) {
    assert(claim_ok && "[failed] SDL_ClaimWindowForGPUDevice\n");
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "failed to claim window for gpu device %s", SDL_GetError());
    return;
  }
}

static void GUIShutdown([[maybe_unused]] flecs::entity e,
                        component::GUI&                comp) {
  SDL_ReleaseWindowFromGPUDevice(comp.renderer.device, comp.window.window);
  SDL_DestroyWindow(comp.window.window);
  SDL_DestroyGPUDevice(comp.renderer.device);
}

GUISystem::GUISystem(flecs::world& world) {
  world.module<GUISystem>();

  world.import <component::GUIComponent>();

  world.observer<component::GUI>().event(flecs::OnSet).each(GUIInitialize);
  world.observer<component::GUI>().event(flecs::OnDelete).each(GUIShutdown);
}
} // namespace wen::system
