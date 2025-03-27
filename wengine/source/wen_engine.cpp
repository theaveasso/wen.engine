#include "wen/wen_engine.hpp"

#include "wen/wen.hpp"

#include "wen/component/wen_pipeline_phase.hpp"
#include "wen/component/wen_window.hpp"
#include "wen/component/wen_renderer.hpp"

#include "wen/system/wen_window.hpp"
#include "wen/system/wen_renderer.hpp"

namespace wen {

static void ShutdownEngine(ecs_world_t* world, void* ctx) {
  auto window = ecs_lookup(world, WEN_WINDOW);
  ecs_clear(world, window);
};

engine::engine() : m_engine(make_unique<flecs::world>()) {
  auto initialize_ok = initialize();
  if (!initialize_ok) {
    assert(initialize_ok && "failed to initialize wen");
    return;
  }
}

engine::~engine() {
  SDL_Quit();
}

bool engine::initialize() {
  bool init_ok = SDL_Init(SDL_INIT_VIDEO);
  if (!init_ok) {
    assert(init_ok && "failed SDL_INIT");
    return false;
  }

  m_engine->import <component::PipelinePhaseComponent>();

  m_engine->import <system::WindowSystem>();
  m_engine->import <system::RendererSystem>();

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

  auto app = m_engine->entity(WEN_APP);

  app.emplace<component::WindowConfig>(1280, 720);
  app.add<component::Renderer>();

  return true;
}

void engine::run() {
  // m_engine->atfini(ShutdownEngine);

  while (!m_engine->should_quit()) {
    m_engine->progress();
  }
}
} // namespace wen
