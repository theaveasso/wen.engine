#ifndef WEN_ENGINE_HPP_
#define WEN_ENGINE_HPP_

#include <cstddef>
#include <flecs.h>

#include "wen/wen_defines.hpp"

namespace wen {

struct position {};
struct velocity {};

class WEN_API_EXPORT engine {
public:
  engine();
  ~engine() = default;

  bool initialize();
  void run();

private:
  std::unique_ptr<flecs::world> m_engine;

  flecs::query<> m_on_process_event_q;
  flecs::query<> m_on_draw_q;
  flecs::query<> m_on_update_q;
  flecs::query<> m_on_begin_draw_q;
  flecs::query<> m_on_end_draw_q;

private:
  template <typename T> flecs::query<> create_query() {
    return m_engine->query_builder<>()
        .with<T>()
        .order_by(0, [](flecs::entity_t e1, const void* pt1, flecs::entity_t e2,
                        const void* pt2) { return (e1 > e2) - (e1 < e2); })
        .build();
  }
};
} // namespace wen

#endif // WEN_ENGINE_HPP_
