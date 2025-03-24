#include "wen/system/renderer.hpp"

#include "wen/component/gui.hpp"
#include "wen/component/renderer.hpp"

namespace wen::system {
static SDL_GPUShader* load_shader() {
  return nullptr;
}

static void BeginFrame(flecs::iter it, size_t i, component::FrameData& comp) {
  SDL_GPUShader* vert_shader = load_shader();
  SDL_GPUShader* frag_shader = load_shader();

  auto gui = it.world().get<component::GUI>();

  SDL_GPUColorTargetDescription color_target_desc[1] = {
      {.format = SDL_GetGPUSwapchainTextureFormat(gui->renderer.device,
                                                  gui->window.window)}};

  SDL_GPUGraphicsPipelineCreateInfo pipeline_create_info = {
      .vertex_shader   = vert_shader,
      .fragment_shader = frag_shader,
      .primitive_type  = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
      .target_info =
          {
              .color_target_descriptions = color_target_desc,
              .num_color_targets         = 1,
          },
  };

  pipeline_create_info.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
  comp.pipeline = SDL_CreateGPUGraphicsPipeline(gui->renderer.device,
                                                &pipeline_create_info);
  if (!comp.pipeline) {
    return;
  }

  SDL_ReleaseGPUShader(gui->renderer.device, vert_shader);
  SDL_ReleaseGPUShader(gui->renderer.device, frag_shader);
}

static void OnDraw(flecs::iter it, size_t i, component::FrameData& comp) {
  auto gui = it.world().get<component::GUI>();

  comp.command_buffer = SDL_AcquireGPUCommandBuffer(gui->renderer.device);
  if (!comp.command_buffer) {
    assert(comp.command_buffer && "");
    return;
  }

  auto wait_and_acquire_gpu_st_ok = SDL_WaitAndAcquireGPUSwapchainTexture(
      comp.command_buffer, gui->window.window, &comp.swapchain_texture, nullptr,
      nullptr);
  if (!wait_and_acquire_gpu_st_ok) {
    return;
  }

  if (!comp.swapchain_texture) {
    SDL_GPUColorTargetInfo color_target_info = {};
    color_target_info.texture                = comp.swapchain_texture;
    color_target_info.clear_color = SDL_FColor{0.0f, 0.0f, 0.0f, 1.0f};
    color_target_info.load_op     = SDL_GPU_LOADOP_CLEAR;
    color_target_info.store_op    = SDL_GPU_STOREOP_STORE;

    SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(
        comp.command_buffer, &color_target_info, 1, nullptr);
    SDL_BindGPUGraphicsPipeline(render_pass, comp.pipeline);
    SDL_DrawGPUPrimitives(render_pass, 3, 1, 0, 0);
    SDL_EndGPURenderPass(render_pass);
  }
}

static void EndFrame(flecs::entity e, component::FrameData& comp) {
  SDL_SubmitGPUCommandBuffer(comp.command_buffer);
}

RendererSystem::RendererSystem(flecs::world& world) {
  world.module<RendererSystem>();

  world.import <component::RendererComponent>();

  world.observer<component::FrameData>().event(flecs::OnSet).each(BeginFrame);
  flecs::entity on_begin_frame_pipeline = world.pipeline()
                                        .with(flecs::System)
                                        .with(component::OnBeginFrame)
                                        .build();
  world.set_pipeline(on_begin_frame_pipeline);

  flecs::entity on_end_frame_pipeline = world.pipeline()
      .with(flecs::System)
      .with(component::OnEndFrame)
      .build();
  world.set_pipeline(on_end_frame_pipeline);

  world.system<component::FrameData>().kind(on_begin_frame_pipeline).each(BeginFrame);
  //
  //  world.system<component::FrameData>().kind().each(EndFrame);
}
} // namespace wen::system