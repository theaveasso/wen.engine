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

static void LoadGPUShader(flecs::iter& it, size_t i,
                          component::GPUShader& shader_comp) {
  auto gui = it.world().get<component::GUI>();
  assert(gui && "null gui component");

  SDL_GPUShaderStage stage;
  if (SDL_strstr(shader_comp.filename, ".vert")) {
    stage = SDL_GPU_SHADERSTAGE_VERTEX;
  } else if (SDL_strstr(shader_comp.filename, ".frag")) {
    stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
  } else {
    assert("failed invalid shader stage!");
    return;
  }

  std::string fullpath;
  const char* entrypoint;

  SDL_GPUShaderFormat backend_fmt =
      SDL_GetGPUShaderFormats(gui->renderer.device);
  SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_INVALID;

  auto base_path = SDL_GetBasePath();
  if (backend_fmt & SDL_GPU_SHADERFORMAT_SPIRV) {
    fullpath = std::string(base_path) + "etc/shaders/compiled/spirv/" + std::string(shader_comp.filename) + ".spirv";
    entrypoint = "main";
    format     = SDL_GPU_SHADERFORMAT_SPIRV;
  } else if (backend_fmt & SDL_GPU_SHADERFORMAT_MSL) {
    fullpath = std::string(base_path) + "etc/shaders/compiled/msl/" + std::string(shader_comp.filename) + ".msl";
    format     = SDL_GPU_SHADERFORMAT_MSL;
    entrypoint = "main0";
  } else if (backend_fmt & SDL_GPU_SHADERFORMAT_DXIL) {
    fullpath = std::string(base_path) + "etc/shaders/compiled/dxil/" + std::string(shader_comp.filename) + ".dxil";
    format     = SDL_GPU_SHADERFORMAT_DXIL;
    entrypoint = "main";
  } else {
    assert("failed unrecognized backend shader format");
    return;
  }

  size_t code_size;
  void*  code = SDL_LoadFile(fullpath.c_str(), &code_size);
  if (!code) {
    assert("failed SDL_LoadFile");
    return;
  }

  SDL_GPUShaderCreateInfo shader_info = {
      .code_size    = code_size,
      .code         = static_cast<const Uint8*>(code),
      .entrypoint   = entrypoint,
      .format       = format,
      .stage        = stage,
      .num_samplers = static_cast<Uint32>(shader_comp.sampler_count),
      .num_storage_textures =
          static_cast<Uint32>(shader_comp.storage_texture_count),
      .num_storage_buffers =
          static_cast<Uint32>(shader_comp.storage_buffer_count),
      .num_uniform_buffers =
          static_cast<Uint32>(shader_comp.uniform_buffer_count),
  };

  shader_comp.shader = SDL_CreateGPUShader(gui->renderer.device, &shader_info);
  if (!shader_comp.shader) {
    assert(shader_comp.shader && "failed SDL_CreateGPUShader");
    SDL_free(code);
    return;
  }

  SDL_free(code);
}

RendererSystem::RendererSystem(flecs::world& world) {
  world.module<RendererSystem>();

  world.import <component::RendererComponent>();

  world.observer<component::GPUShader>()
      .event(flecs::OnSet)
      .each(LoadGPUShader);
}
} // namespace wen::system
