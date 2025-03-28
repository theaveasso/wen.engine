#include "wen/system/wen_renderer.hpp"

#include "wen/component/wen_renderer.hpp"
#include "wen/component/wen_window.hpp"
#include "wen/wen.hpp"

namespace wen::system {
static component::RenderPassData InitFrameData(SDL_GPUDevice* gpu) {
  component::RenderPassData result;

  auto vtb_create_info = SDL_GPUBufferCreateInfo{
      .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
      .size  = sizeof(component::PositionTextureVertex) * 4,
  };
  auto idx_create_info = SDL_GPUBufferCreateInfo{
      .usage = SDL_GPU_BUFFERUSAGE_VERTEX, .size = sizeof(Uint16) * 6};
  auto tex_create_info = SDL_GPUTextureCreateInfo{
      .type                 = SDL_GPU_TEXTURETYPE_2D,
      .format               = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
      .usage                = SDL_GPU_TEXTUREUSAGE_SAMPLER,
      .width                = 1, // TODO
      .height               = 1,
      .layer_count_or_depth = 1,
      .num_levels           = 1,
  };
  result.quad_vertex_buffer  = SDL_CreateGPUBuffer(gpu, &vtb_create_info);
  result.quad_indices_buffer = SDL_CreateGPUBuffer(gpu, &idx_create_info);

  auto texture = SDL_CreateGPUTexture(gpu, &tex_create_info);
  SDL_SetGPUTextureName(gpu, texture, "Noise Texture");
  // auto noise_texture = SDL_CreateGPUTexture();

  return result;
}

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

static void InitRenderPass(flecs::iter& it) {
  while (it.next()) {
    auto frame_data = it.field<component::RenderPassData>(0);
    auto engine     = it.entity(0).lookup(WEN_RENDERER);

    auto renderer = engine.get_ref<component::Renderer>();
    auto window   = engine.get_ref<component::Window>();

    frame_data->command_buffer = SDL_AcquireGPUCommandBuffer(renderer->gpu);
    if (frame_data->command_buffer) {
      return;
    }

    bool acquire_gpu_st_ok = SDL_WaitAndAcquireGPUSwapchainTexture(
        frame_data->command_buffer, window->window,
        &frame_data->swapchain_texture, nullptr, nullptr);
    if (!acquire_gpu_st_ok) {
      return;
    }
  }
}

static void DrawFrame(flecs::entity, component::RenderPassData& data) {
  if (data.swapchain_texture) {
    data.color_target_info             = {};
    data.color_target_info.texture     = data.swapchain_texture;
    data.color_target_info.clear_color = SDL_FColor{0.0f, 0.0f, 0.0f, 1.0f};
    data.color_target_info.load_op     = SDL_GPU_LOADOP_CLEAR;
    data.color_target_info.store_op    = SDL_GPU_STOREOP_STORE;

    auto render_pass = SDL_BeginGPURenderPass(
        data.command_buffer, &data.color_target_info, 1, nullptr);

    SDL_GPUBufferBinding gpu_buffer_binding = {
        .buffer = data.quad_vertex_buffer, .offset = 0};
    SDL_BindGPUGraphicsPipeline(render_pass, data.pipeline);
    SDL_BindGPUVertexBuffers(render_pass, 0, &gpu_buffer_binding, 1);
    SDL_DrawGPUPrimitives(render_pass, 3, 1, 0, 0);
    SDL_EndGPURenderPass(render_pass);
  }
}

static void PresentFrame(flecs::entity, component::RenderPassData& data) {
  SDL_SubmitGPUCommandBuffer(data.command_buffer);
}

RendererSystem::RendererSystem(flecs::world& world) {
  world.module<RendererSystem>();

  world.import <component::RendererComponent>();

  world.observer<component::Renderer>().event(flecs::OnAdd).each(InitRenderer);

  world.observer<component::Renderer>()
      .event(flecs::OnRemove)
      .each(ShutdownRenderer);

  // base renderer scope
  world.scope(world.entity(WEN_RENDERER), [&]() {
    world.module<BaseRenderer>("base_renderer");

    world.system<component::RenderPassData>()
        .kind(flecs::OnLoad)
        .immediate()
        .run(InitRenderPass);

    world.system<component::RenderPassData>()
        .kind(flecs::OnStore)
        .each(DrawFrame);

    world.system<component::RenderPassData>()
        .kind(flecs::OnStore)
        .each(PresentFrame);
  });

  // defer renderer scope
  world.scope(world.entity(WEN_RENDERER),
              [&]() { world.module<DeferedRenderer>("defered_renderer"); });

  // forward renderer scope
  world.scope(world.entity(WEN_RENDERER),
              [&]() { world.module<ForwardRenderer>("forward_renderer"); });
}
} // namespace wen::system
