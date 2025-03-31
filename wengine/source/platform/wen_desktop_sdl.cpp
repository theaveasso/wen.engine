#include "wen/platform/wen_platform.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include "wen/core/wen_logger.hpp"
#include "wen/core/wen_input.hpp"
#include "wen/datastructures/wen_vec.hpp"
#include "wen/core/wen_memory.hpp"

typedef struct sdl_platform_data_t {
  SDL_Window*    window;
  SDL_GPUDevice* gpu_device;
} sdl_platform_data_t;

static bool is_running = true;

bool platform_init(wen_platform_state_t* state_, const char* name_, int32_t width_, int32_t height_) {
  state_->internal_state = wen_memalloc(sizeof(sdl_platform_data_t), MEMORY_TAG_APPLICATION);
  auto* state            = (sdl_platform_data_t*)state_->internal_state;

  bool result = SDL_Init(SDL_INIT_VIDEO);
  if (!result) {
    return false;
  }

  SDL_WindowFlags window_flags = 0;
  window_flags |= SDL_WINDOW_HIGH_PIXEL_DENSITY;
  window_flags |= SDL_WINDOW_VULKAN;

  state->window = SDL_CreateWindow(name_, width_, height_, window_flags);
  if (!state->window) {
    return false;
  }

  SDL_GPUShaderFormat shader_formats = 0;
  shader_formats |= SDL_GPU_SHADERFORMAT_SPIRV;
  shader_formats |= SDL_GPU_SHADERFORMAT_DXIL;
  shader_formats |= SDL_GPU_SHADERFORMAT_DXBC;
  shader_formats |= SDL_GPU_SHADERFORMAT_MSL;
  state->gpu_device = SDL_CreateGPUDevice(shader_formats, true, nullptr);
  if (!state->gpu_device) {
    return false;
  }

  result = SDL_ClaimWindowForGPUDevice(state->gpu_device, state->window);
  if (!result) {
    return false;
  }

  return true;
}

void platform_get_required_exts_names(const char*** exts_list) {
  /** SDL automatically handle. */
}

void platform_shutdown(wen_platform_state_t* state_) {
  auto* state = (sdl_platform_data_t*)state_->internal_state;
  SDL_WaitForGPUIdle(state->gpu_device);
  SDL_ReleaseWindowFromGPUDevice(state->gpu_device, state->window);
  SDL_DestroyGPUDevice(state->gpu_device);
  SDL_DestroyWindow(state->window);
  SDL_Quit();
  wen_memfree(state, sizeof(sdl_platform_data_t), MEMORY_TAG_APPLICATION);
}

void platform_sleep(uint32_t milliseconds) {
  SDL_Delay(milliseconds);
}

void platform_poll_event() {
  SDL_Event event{};
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_EVENT_QUIT:
      is_running = false;
      break;
    case SDL_EVENT_KEY_DOWN:
      input_process_key((keys_t)event.key.key, true);
      break;
    case SDL_EVENT_KEY_UP:
      input_process_key((keys_t)event.key.key, false);
      break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
      if (event.button.button <= BUTTON_COUNT) {
        if (event.button.button == 1) {
          input_process_button(BUTTON_LEFT, true);
        } else if (event.button.button == 2) {
          input_process_button(BUTTON_MIDDLE, true);
        } else if (event.button.button == 3) {
          input_process_button(BUTTON_RIGHT, true);
        }
      }
      break;
    case SDL_EVENT_MOUSE_BUTTON_UP:
      if (event.button.button <= BUTTON_COUNT) {
        if (event.button.button == 1) {
          input_process_button(BUTTON_LEFT, false);
        } else if (event.button.button == 2) {
          input_process_button(BUTTON_MIDDLE, false);
        } else if (event.button.button == 3) {
          input_process_button(BUTTON_RIGHT, false);
        }
      }
      break;
    case SDL_EVENT_MOUSE_MOTION:
      input_process_mouse_move((int16_t)event.button.x, (int16_t)event.button.y);
      break;
    case SDL_EVENT_MOUSE_WHEEL:
      auto z_delta = (int8_t)(event.wheel.y);
      input_process_mouse_wheel(z_delta < 0 ? -1 : 1);
      break;
    }
  }
}

bool platform_is_running() {
  return is_running;
}

double platform_get_absolute_time() {
  return (double)SDL_GetTicks();
};