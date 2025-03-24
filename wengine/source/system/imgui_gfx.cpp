//#include "wen/system/imgui_gfx.hpp"
//
//#include <backends/imgui_impl_sdl3.h>
//#include <backends/imgui_impl_sdlgpu3.h>
//#include <imgui.h>
//
//#include "wen/component/imgui_gfx.hpp"
//#include "wen/component/input.hpp"
//#include "wen/component/renderer.hpp"
//#include "wen/component/window.hpp"
//
//namespace wen::system {
//using namespace component;
//static void ImGuiInitialize(flecs::iter& itr, size_t i,
//                            ImGui_GFXComponent&    imgui_comp,
//                            WindowComponent&       window_comp,
//                            SDL_RendererComponent& renderer_comp,
//                            InputComponent&        input_comp) {
//  if (window_comp.window == nullptr) {
//    ecs_err("SDL_Window not found! Make sure that SDL_System was added.");
//    return;
//  }
//
//  IMGUI_CHECKVERSION();
//  imgui_comp.ctx = ImGui::CreateContext();
//  imgui_comp.io  = &ImGui::GetIO();
//  (void)imgui_comp.io;
//
//  imgui_comp.io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
//  imgui_comp.io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
//  imgui_comp.io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
//  imgui_comp.io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
//
//  ImGui::StyleColorsDark();
//
//  ImGui_ImplSDL3_InitForSDLGPU(window_comp.window);
//
//  ImGui_ImplSDLGPU3_InitInfo init_info = {};
//  init_info.Device                     = renderer_comp.gpu_device;
//  init_info.ColorTargetFormat          = SDL_GetGPUSwapchainTextureFormat(
//      renderer_comp.gpu_device, window_comp.window);
//  init_info.MSAASamples = SDL_GPU_SAMPLECOUNT_1;
//  ImGui_ImplSDLGPU3_Init(&init_info);
//}
//
//static void
//ImGuiShutdown([[maybe_unused]] flecs::entity          e,
//              [[maybe_unused]] SDL_RendererComponent& renderer_comp) {
//  ImGui_ImplSDL3_Shutdown();
//  ImGui_ImplSDLGPU3_Shutdown();
//  ImGui::DestroyContext();
//}
//
//static void ImGuiUpdate(flecs::iter& itr, size_t i,
//                        WindowComponent&       window_comp,
//                        SDL_RendererComponent& renderer_comp) {
//  ImGui_ImplSDLGPU3_NewFrame();
//  ImGui_ImplSDL3_NewFrame();
//  ImGui::NewFrame();
//
//  ImGui::DockSpaceOverViewport(0, nullptr,
//                               ImGuiDockNodeFlags_PassthruCentralNode);
//
//  bool show = true;
//  ImGui::ShowDemoWindow(&show);
//  {
//    static float f       = 0.0f;
//    static int   counter = 0;
//
//    ImGui::Begin("Hello world");
//
//    ImGui::Text("This is some useful text.");
//    ImGui::Checkbox("Demo Window", &show);
//    ImGui::Checkbox("Another Window", &show);
//
//    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
//    ImGui::End();
//  }
//
//  ImGui::Render();
//  ImDrawData* draw_data = ImGui::GetDrawData();
//  const bool  is_minimized =
//      (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);
//
//  SDL_GPUCommandBuffer* cmd_buffer =
//      SDL_AcquireGPUCommandBuffer(renderer_comp.gpu_device);
//
//  SDL_GPUTexture* swapchain_texture;
//  SDL_AcquireGPUSwapchainTexture(cmd_buffer, window_comp.window,
//                                 &swapchain_texture, nullptr, nullptr);
//
//  if (swapchain_texture != nullptr && !is_minimized) {
//    Imgui_ImplSDLGPU3_PrepareDrawData(draw_data, cmd_buffer);
//
//    SDL_GPUColorTargetInfo target_info = {};
//    target_info.texture                = swapchain_texture;
//    target_info.clear_color            = SDL_FColor{0.45, 0.55, 0.60, 1.0};
//    target_info.load_op                = SDL_GPU_LOADOP_CLEAR;
//    target_info.store_op               = SDL_GPU_STOREOP_STORE;
//    target_info.mip_level              = 0;
//    target_info.cycle                  = false;
//
//    SDL_GPURenderPass* render_pass =
//        SDL_BeginGPURenderPass(cmd_buffer, &target_info, 1, nullptr);
//    ImGui_ImplSDLGPU3_RenderDrawData(draw_data, cmd_buffer, render_pass);
//    SDL_EndGPURenderPass(render_pass);
//  }
//  SDL_SubmitGPUCommandBuffer(cmd_buffer);
//}
//
//ImGui_GFXSystem::ImGui_GFXSystem(flecs::world& world) {
//  world.module<ImGui_GFXSystem>();
//
//  world.import <Input>();
//  world.import <Window>();
//  world.import <ImGui_GFX>();
//  world.import <SDL_Renderer>();
//
//  world
//      .observer<ImGui_GFXComponent, WindowComponent, SDL_RendererComponent,
//                InputComponent>()
//      .event(flecs::OnSet)
//      .each(ImGuiInitialize);
//
//  world.observer<SDL_RendererComponent>()
//      .event(flecs::OnRemove)
//      .each(ImGuiShutdown);
//
//  world.system<WindowComponent, SDL_RendererComponent>()
//      .kind(flecs::OnUpdate)
//      .each(ImGuiUpdate);
//}
//} // namespace wen::system