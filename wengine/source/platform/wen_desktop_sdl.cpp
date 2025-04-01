#include "wen/platform/wen_platform.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include "wen/core/wen_event.hpp"
#include "wen/core/wen_input.hpp"
#include "wen/core/wen_logger.hpp"
#include "wen/core/wen_memory.hpp"

#include "wen/datastructures/wen_vec.hpp"

#include "wen/renderer/vulkan/vulkan_types.inl"

typedef struct sdl_platform_data_t {
  SDL_Window*    window;
  SDL_GPUDevice* gpu_device;
  VkSurfaceKHR   surface;
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
  window_flags |= SDL_WINDOW_RESIZABLE;
  window_flags |= SDL_WINDOW_VULKAN;

  SDL_Vulkan_LoadLibrary(nullptr);
  state->window = SDL_CreateWindow(name_, width_, height_, window_flags);
  if (!state->window) {
    return false;
  }
  return true;
}

void platform_get_required_exts_names(const char*** required_extensions_) {
  uint32_t           count_instance_extensions = 0;
  const char* const* instance_extensions       = SDL_Vulkan_GetInstanceExtensions(&count_instance_extensions);
  if (!instance_extensions) {
    wen_error("failed to get instance extensions.");
    return;
  }
  for (int i = 0; i < count_instance_extensions; ++i) {
    vec_push(*required_extensions_, instance_extensions[i]);
  }
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
    case SDL_EVENT_WINDOW_RESIZED:
      event_context_t resize_ctx;
      resize_ctx.data.u32[0] = event.window.data1;
      resize_ctx.data.u32[1] = event.window.data2;
      event_fire(WEN_EVENT_CODE_WINDOW_RESIZED, nullptr, resize_ctx);
      break;
    case SDL_EVENT_WINDOW_MINIMIZED:
      event_context_t minimize_ctx;
      minimize_ctx.data.c[0] = 1;
      event_fire(WEN_EVENT_CODE_WINDOW_MINIMIZED, nullptr, minimize_ctx);
      break;
    case SDL_EVENT_WINDOW_MOVED:
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
}

bool vulkan_surface_init(wen_vulkan_context_t* context_, wen_platform_state_t* state_) {
  auto*        state = (sdl_platform_data_t*)state_->internal_state;
  VkSurfaceKHR surface;
  bool         result = SDL_Vulkan_CreateSurface(state->window, context_->instance, context_->allocator, &surface);
  if (!result) {
    wen_fatal("[SDL - Vulkan] - failed creating surface. %s", SDL_GetError());
    return false;
  }
  context_->surface = surface;
  state->surface    = surface;
  return true;
}

void vulkan_surface_fini(wen_vulkan_context_t* context_) {
  SDL_Vulkan_DestroySurface(context_->instance, context_->surface, context_->allocator);
}
