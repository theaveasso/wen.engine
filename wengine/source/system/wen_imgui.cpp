#include "wen/system/wen_imgui.hpp"

#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlgpu3.h>
#include <imgui.h>

#include "wen/component/wen_imgui.hpp"
#include "wen/component/wen_renderer.hpp"
#include "wen/component/wen_window.hpp"

namespace wen::system {
using namespace component;
static void InitImgui(flecs::entity e, Imgui& imgui) {
  auto window   = e.get_ref<component::Window>();
  auto renderer = e.get_ref<component::Renderer>();

  if (window->window == nullptr) {
    ecs_err("SDL_Window not found! Make sure that SDL_System was added.");
    return;
  }

  IMGUI_CHECKVERSION();
  imgui.ctx = ImGui::CreateContext();
  imgui.io  = &ImGui::GetIO();
  (void)imgui.io;

  imgui.io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  imgui.io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  imgui.io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  imgui.io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

  ImGui::StyleColorsDark();

  ImGui_ImplSDL3_InitForSDLGPU(window->window);

  ImGui_ImplSDLGPU3_InitInfo init_info = {};
  init_info.Device                     = renderer->gpu;
  init_info.ColorTargetFormat =
      SDL_GetGPUSwapchainTextureFormat(renderer->gpu, window->window);
  init_info.MSAASamples = SDL_GPU_SAMPLECOUNT_1;
  ImGui_ImplSDLGPU3_Init(&init_info);
}

static void ShutdownImgui(flecs::entity e, component::Imgui& imgui) {
  // ImGui_ImplSDL3_Shutdown();
  // ImGui_ImplSDLGPU3_Shutdown();
  // ImGui::DestroyContext();
}

static void UpdateImgui(flecs::iter& it) {
  while (it.next()) {
    auto window   = it.field<component::Window>(0);
    auto renderer = it.field<component::Renderer>(1);

    ImGui_ImplSDLGPU3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport(0, nullptr,
                                 ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();
    const bool  is_minimized =
        (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);

    SDL_GPUCommandBuffer* cmd_buffer =
        SDL_AcquireGPUCommandBuffer(renderer->gpu);

    SDL_GPUTexture* swapchain_texture;
    SDL_AcquireGPUSwapchainTexture(cmd_buffer, window->window,
                                   &swapchain_texture, nullptr, nullptr);

    if (swapchain_texture != nullptr && !is_minimized) {
      Imgui_ImplSDLGPU3_PrepareDrawData(draw_data, cmd_buffer);

      SDL_GPUColorTargetInfo target_info = {};
      target_info.texture                = swapchain_texture;
      target_info.clear_color            = SDL_FColor{0.45, 0.55, 0.60, 1.0};
      target_info.load_op                = SDL_GPU_LOADOP_CLEAR;
      target_info.store_op               = SDL_GPU_STOREOP_STORE;
      target_info.mip_level              = 0;
      target_info.cycle                  = false;

      SDL_GPURenderPass* render_pass =
          SDL_BeginGPURenderPass(cmd_buffer, &target_info, 1, nullptr);
      ImGui_ImplSDLGPU3_RenderDrawData(draw_data, cmd_buffer, render_pass);
      SDL_EndGPURenderPass(render_pass);
    }
    SDL_SubmitGPUCommandBuffer(cmd_buffer);
  }
}

ImguiSystem::ImguiSystem(flecs::world& world) {
  world.module<ImguiSystem>();

  world.import <WindowComponent>();
  world.import <RendererComponent>();

  world.observer<Imgui>().event(flecs::OnAdd).each(InitImgui);

  world.observer<Imgui>().event(flecs::OnRemove).each(ShutdownImgui);

  world.system<Window, Renderer>()
      .kind(flecs::OnUpdate)
      .run(UpdateImgui);
}
} // namespace wen::system
