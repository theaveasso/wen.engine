#include "wen/wen_engine.hpp"

#include "wen/core/wen_logger.hpp"
#include "wen/core/wen_memory.hpp"

#include "wen/wen_game_type.hpp"

#include <SDL3/SDL.h>
#include <flecs.h>
#include <string>

typedef struct engine_state_t {
  game_t*      game_inst;
  ecs_world_t* world;
  bool         is_suspended;
  bool         is_running;
  float        last_time;
  int16_t      width;
  int16_t      height;
} engine_state_t;

static bool           initialized = false;
static engine_state_t app_state{};

bool engine_create(game_t* game_inst) {
  if (initialized) {
    WEN_ERROR("engine is already initialized.");
    return false;
  }
  app_state.game_inst = game_inst;

  app_state.is_running   = true;
  app_state.is_suspended = false;

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    WEN_ERROR("failed sdl init SDL_INIT_VIDEO.");
    return false;
  }

  if (!app_state.game_inst->initialize(app_state.game_inst)) {
    WEN_ERROR("failed initialize game instance.");
    return false;
  }
  app_state.game_inst->on_resize(app_state.game_inst, app_state.width,
                                 app_state.height);

  initialized = true;
  return true;
}

bool engine_run() {
  WEN_TRACE("%s", get_mem_usage_str().c_str());
  while (app_state.is_running) {
    if (app_state.is_suspended) {
      return false;
    }

    if (!app_state.game_inst->update(app_state.game_inst, 0)) {
      WEN_FATAL("game update failed, shutting down.");
      app_state.is_running = false;
      return false;
    }

    if (!app_state.game_inst->render(app_state.game_inst, 0)) {
      WEN_FATAL("game render failed, shutting down.");
      app_state.is_running = false;
      return false;
    }
  }

  app_state.is_running = false;
  return true;
}

// namespace wen {

// static void ShutdownEngine(ecs_world_t* world, void* ctx) {};
//
// engine::engine() : m_engine(make_unique<flecs::world>()) {
//   auto initialize_ok = initialize();
//   if (!initialize_ok) {
//     assert(initialize_ok && "failed to initialize wen");
//     return;
//   }
// }
//
// engine::~engine() {
//   SDL_Quit();
// }

// bool engine::initialize() {
//   bool init_ok = SDL_Init(SDL_INIT_VIDEO);
//   if (!init_ok) {
//     assert(init_ok && "failed SDL_INIT");
//     return false;
//   }
//
//   m_engine->import <component::PipelinePhaseComponent>();
//
//   m_engine->import <system::WindowSystem>();
//   m_engine->import <system::RendererSystem>();
//   m_engine->import <system::ImguiSystem>();
//   m_engine->import <system::InputSystem>();
//
//   m_on_process_event_q = create_query<component::OnProcessEvent>();
//   m_on_draw_q          = create_query<component::OnDraw>();
//   m_on_update_q        = create_query<component::OnUpdate>();
//   m_on_begin_draw_q    = create_query<component::OnBeginDraw>();
//   m_on_end_draw_q      = create_query<component::OnEndDraw>();
//
//   auto on_process_event_phase =
//   m_engine->entity<component::OnProcessEvent>(); auto on_draw_phase =
//   m_engine->entity<component::OnDraw>(); auto on_update_phase        =
//   m_engine->entity<component::OnUpdate>(); auto on_begin_draw_phase    =
//   m_engine->entity<component::OnBeginDraw>(); auto on_end_draw_phase      =
//   m_engine->entity<component::OnEndDraw>();
//
//   auto app = m_engine->entity(WEN_APP);
//
//   app.emplace<component::WindowConfig>(1280, 720);
//   app.add<component::Renderer>();
//   app.add<component::Imgui>();
//   app.add<component::Input>();
//
//   return true;
// }

// void engine::run() {
//   // m_engine->atfini(ShutdownEngine);
//   m_engine->app().enable_stats().enable_rest(3333).delta_time(1.0
//   / 60.0).run();
// }
// } // namespace wen
