#include "wen/wen_engine.hpp"

#include "wen/component/gui.hpp"
#include "wen/component/input.hpp"
#include "wen/component/renderer.hpp"
#include "wen/system/gui.hpp"
#include "wen/system/input.hpp"
#include "wen/system/renderer.hpp"

namespace wen {

static void OnDestroy(ecs_world_t* world, void* ctx) {
  SDL_Quit();
};

engine::engine() : m_world(make_unique<flecs::world>()) {
  auto initialize_ok = initialize();
  if (!initialize_ok) {
    assert(initialize_ok && "failed to initialize wen");
    return;
  }
}

bool engine::initialize() {
  m_world->import <system::GUISystem>();
  m_world->import <system::RendererSystem>();

  m_world->import <component::InputComponent>();

  bool init_ok = SDL_Init(SDL_INIT_VIDEO);
  if (!init_ok) {
    assert(init_ok && "failed SDL_INIT");
    return false;
  }

  m_world->set<component::GUI>({});
  m_world->set<component::Input>({});

  m_world->component<OnProcessEvent>();
  m_world->component<OnDraw>();
  m_world->component<OnUpdate>();
  m_world->component<OnBeginDraw>();
  m_world->component<OnEndDraw>();

  m_on_process_event_q = create_query<OnProcessEvent>();
  m_on_draw_q          = create_query<OnDraw>();
  m_on_update_q        = create_query<OnUpdate>();
  m_on_begin_draw_q    = create_query<OnBeginDraw>();
  m_on_end_draw_q      = create_query<OnEndDraw>();

  m_world->entity("test-shader")
      .emplace<component::GPUShader>("raw_triangle.vert", 0, 0, 0, 0);

  return true;
}

void engine::run() {
  m_world->atfini(OnDestroy);

  m_world->system<component::Input>()
      .kind(flecs::PostLoad)
      .each(system::ProcessEvent);

  m_world->app().delta_time(1.0 / 60).run();
}
} // namespace wen
