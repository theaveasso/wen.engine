#include "wen/system/wen_renderer.hpp"

#include "wen/component/wen_renderer.hpp"
#include "wen/component/wen_window.hpp"

namespace wen::system {
static void InitRenderer(flecs::entity e, component::Renderer& r) {
  auto window = e.get_ref<component::Window>();
  if (!window) {
    return;
  }

  int support_shader_format;
  support_shader_format |= SDL_GPU_SHADERFORMAT_SPIRV;
  support_shader_format |= SDL_GPU_SHADERFORMAT_DXIL;
  support_shader_format |= SDL_GPU_SHADERFORMAT_MSL;

  r.gpu = SDL_CreateGPUDevice(support_shader_format, true, nullptr);
  if (!r.gpu) {
    return;
  }

  bool claim_ok = SDL_ClaimWindowForGPUDevice(r.gpu, window->window);
  if (!claim_ok) {
    return;
  }
}

static void ShutdownRenderer(flecs::entity e, component::Renderer& r) {
  // auto window = e.get_ref<component::Window>();
  // if (!window) {
  //   return;
  // }
  //
  // SDL_ReleaseWindowFromGPUDevice(r.gpu, window->window);
  // SDL_DestroyGPUDevice(r.gpu);
}

RendererSystem::RendererSystem(flecs::world& world) {
  world.module<RendererSystem>();

  world.import <component::RendererComponent>();

  world.observer<component::Renderer>().event(flecs::OnAdd).each(InitRenderer);

  world.observer<component::Renderer>()
      .event(flecs::OnRemove)
      .each(ShutdownRenderer);
} 
} // namespace wen::system
