#include "wen/core/wen_engine.hpp"

#include <cmath>

#include "wen/core/wen_clock.hpp"
#include "wen/core/wen_event.hpp"
#include "wen/core/wen_input.hpp"
#include "wen/core/wen_logger.hpp"
#include "wen/core/wen_memory.hpp"

#include "wen/platform/wen_platform.hpp"

#include "wen/renderer/wen_renderer_frontend.hpp"

#include "wen/wen_game_type.hpp"

typedef struct wen_engine_t {
  const char* title;
  uint64_t    flags;

  bool fullscreen;
  bool is_running;
  bool is_suspended;

  double last_time;

  wen_point_t          position;
  wen_clock_t          clock;
  wen_game_t*          game_inst;
  wen_platform_state_t platform_state;
} wen_engine_t;

bool engine_on_event(uint16_t code, void* sender, void* listener, event_context_t ctx);
bool engine_on_key(uint16_t code, void* sender, void* listener, event_context_t ctx);

static bool         initialized = false;
static wen_engine_t engine_state{};

bool engine_create(wen_game_t* game_inst) {
  if (initialized) {
    return false;
  }

  engine_state.game_inst = game_inst;

  engine_state.is_running   = true;
  engine_state.is_suspended = false;

  log_system_initialize();
  input_system_initialize();

  if (!event_system_initialize()) {
    wen_error("fail initializing event system!");
    return false;
  }

  event_register(WEN_EVENT_CODE_APPLICATION_QUIT, nullptr, engine_on_event);
  event_register(WEN_EVENT_CODE_BUTTON_RELEASED, nullptr, engine_on_key);
  event_register(WEN_EVENT_CODE_KEY_PRESSED, nullptr, engine_on_key);

  if (!platform_init(&engine_state.platform_state, engine_state.title,
                     engine_state.game_inst->app_config.start_width,
                     engine_state.game_inst->app_config.start_height)) {
    return false;
  }
//  if (!renderer_initialize(engine_state.title, &engine_state.platform_state)) {
//    return false;
//  }

  if (!engine_state.game_inst->initialize(engine_state.game_inst)) {
    wen_error("failed initialize game instance.");
    return false;
  }

  engine_state.game_inst->on_resize(engine_state.game_inst,
                                    engine_state.game_inst->app_config.start_width,
                                    engine_state.game_inst->app_config.start_height);

  initialized = true;
  return true;
}

bool engine_run() {
  wen_trace(get_mem_usage_str().c_str());

  clock_start(&engine_state.clock);
  clock_update(&engine_state.clock);
  engine_state.last_time = engine_state.clock.elapsed;

  double  running_time = 0;
  uint8_t frame_count  = 0;
  float   target_fps   = 1.0 / 60;

  while (engine_state.is_running) {
    clock_update(&engine_state.clock);
    double current_time     = engine_state.clock.elapsed;
    double deltatime        = current_time - engine_state.last_time;
    double frame_start_time = platform_get_absolute_time();

    if (!platform_is_running()) {
      engine_state.is_running = false;
    }
    if (engine_state.is_suspended) {
      return false;
    }

    if (!engine_state.game_inst->update(engine_state.game_inst, (float)deltatime)) {
      wen_error("game update failed, shutting down!");
      engine_state.is_running = false;
      return false;
    }

    if (!engine_state.game_inst->render(engine_state.game_inst, 0)) {
      wen_error("game render failed, shutting down.");
      engine_state.is_running = false;
      return false;
    }

//    wen_render_packet_t packet;
//    packet.deltatime = (float)deltatime;
//    renderer_draw_frame(&packet);

    double frame_end_time     = platform_get_absolute_time();
    double frame_elapsed_time = frame_end_time - frame_start_time;

    running_time += frame_elapsed_time;
    double remaining_seconds = target_fps - frame_elapsed_time;
    if (remaining_seconds > 0) {
      uint64_t remaining_ms = (uint64_t)remaining_seconds * 1000;
      // bool     limit_frames = false;
      if (remaining_ms > 0 /** && limit_frames */) {
        platform_sleep((uint32_t)remaining_seconds - 1);
      }
    }
    frame_count++;

    platform_poll_event();
    input_system_update((float)deltatime);

    engine_state.last_time = current_time;
  }

  engine_state.is_running = false;

  event_unregister(WEN_EVENT_CODE_APPLICATION_QUIT, nullptr, engine_on_event);
  event_unregister(WEN_EVENT_CODE_BUTTON_RELEASED, nullptr, engine_on_key);
  event_unregister(WEN_EVENT_CODE_KEY_PRESSED, nullptr, engine_on_key);

  event_system_shutdown();
  input_system_shutdown();

//  renderer_shutdown();
  platform_shutdown(&engine_state.platform_state);

  return true;
}

bool engine_on_event(uint16_t code, void*, void*, event_context_t) {
  if (code == WEN_EVENT_CODE_APPLICATION_QUIT) {
    engine_state.is_running = false;
    return true;
  }
  return false;
}
bool engine_on_key(uint16_t code, void*, void*, event_context_t ctx) {
  if (code == WEN_EVENT_CODE_KEY_PRESSED) {
    uint32_t key_code = ctx.data.u32[0];
    if (key_code == WEN_KESCAPE) {
      event_context_t data{};
      event_fire(WEN_EVENT_CODE_APPLICATION_QUIT, nullptr, data);
      return true;
    }
    return false;
  } else if (code == WEN_EVENT_CODE_BUTTON_RELEASED) {
    return true;
  }

  return false;
}
