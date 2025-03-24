//#include <iostream>
//
//#include "wen/system/renderer.hpp"
//
//#include "wen/component/renderer.hpp"
//#include "wen/component/window.hpp"
//
//namespace wen::system {
//using namespace wen::component;
//
//internal void CreateSDLRenderer([[maybe_unused]] flecs::entity e,
//                                WindowComponent&               window_comp,
//                                SDL_RendererComponent&         renderer_comp) {
//  auto gpu_device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV |
//                                            SDL_GPU_SHADERFORMAT_DXIL |
//                                            SDL_GPU_SHADERFORMAT_METALLIB,
//                                        true, nullptr);
//  if (gpu_device == nullptr) {
//    ecs_err("failed to create SDL_GPUDevice! %s\n", SDL_GetError());
//    return;
//  }
//
//  if (!SDL_ClaimWindowForGPUDevice(gpu_device, window_comp.window)) {
//    ecs_err("failed to claim SDL_Window for SDL_GPUDevice! %s\n",
//            SDL_GetError());
//    return;
//  }
//  SDL_SetGPUSwapchainParameters(gpu_device, window_comp.window,
//                                SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
//                                SDL_GPU_PRESENTMODE_MAILBOX);
//
//  renderer_comp.gpu_device = gpu_device;
//}
//
//internal void DestroySDLRenderer([[maybe_unused]] flecs::entity e,
//                                 WindowComponent&               window_comp,
//                                 SDL_RendererComponent&         renderer_comp) {
//
//  SDL_WaitForGPUIdle(renderer_comp.gpu_device);
//  SDL_ReleaseWindowFromGPUDevice(renderer_comp.gpu_device, window_comp.window);
//  if (renderer_comp.gpu_device != nullptr) {
//    SDL_DestroyGPUDevice(renderer_comp.gpu_device);
//    renderer_comp.gpu_device = nullptr;
//  }
//  SDL_DestroyRenderer(renderer_comp.renderer);
//}
//
//SDL_RendererSystem::SDL_RendererSystem(flecs::world& world) {
//  world.module<SDL_RendererSystem>();
//
//  world.import <Window>();
//  world.import <SDL_Renderer>();
//
//  world.observer<WindowComponent, SDL_RendererComponent>()
//      .event(flecs::OnSet)
//      .each(CreateSDLRenderer);
//
//  world.observer<WindowComponent, SDL_RendererComponent>()
//      .event(flecs::OnRemove)
//      .each(DestroySDLRenderer);
//}
//} // namespace wen::system
// namespace wen::system
