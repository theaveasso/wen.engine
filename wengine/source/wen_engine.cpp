#include "wen/wen_engine.hpp"

#include <iostream>

#include "wen/component/wen_pipeline_phase.hpp"

namespace wen {

static void OnDestroy(ecs_world_t* world, void* ctx) {
  SDL_Quit();
};

engine::engine() : m_engine(make_unique<flecs::world>()) {
  auto initialize_ok = initialize();
  if (!initialize_ok) {
    assert(initialize_ok && "failed to initialize wen");
    return;
  }
}

bool engine::initialize() {
  bool init_ok = SDL_Init(SDL_INIT_VIDEO);
  if (!init_ok) {
    assert(init_ok && "failed SDL_INIT");
    return false;
  }

  m_engine->import <component::PhaseComponent>();

  m_on_process_event_q = create_query<component::OnProcessEvent>();
  m_on_draw_q          = create_query<component::OnDraw>();
  m_on_update_q        = create_query<component::OnUpdate>();
  m_on_begin_draw_q    = create_query<component::OnBeginDraw>();
  m_on_end_draw_q      = create_query<component::OnEndDraw>();

  auto on_process_event_phase = m_engine->entity<component::OnProcessEvent>();
  auto on_draw_phase          = m_engine->entity<component::OnDraw>();
  auto on_update_phase        = m_engine->entity<component::OnUpdate>();
  auto on_begin_draw_phase    = m_engine->entity<component::OnBeginDraw>();
  auto on_end_draw_phase      = m_engine->entity<component::OnEndDraw>();

  auto ccc = m_engine->entity();
  ccc.add<position>();

  m_engine->system<position>()
      .kind(on_process_event_phase)
      .run([](flecs::iter& it) {
        while (it.next()) {
          std::cout << "system - draw" << std::endl;
        }
      });

  return true;
}

void engine::run() {
  m_engine->progress();
}
} // namespace wen
