#include "wen/system/wen_primitive.hpp"

#include "wen/component/wen_primitive.hpp"
#include "wen/component/wen_renderer.hpp"
#include "wen/component/wen_transform.hpp"
#include "wen/component/wen_material.hpp"
#include "wen/wen.hpp"

namespace wen::system {
static void populate_buffers(flecs::world&                world,
                             component::Primitive&        primitive,
                             component::PrimitiveBuffers& buffers,
                             flecs::query<>&              query,
                             const component::Materials&  material_data) {
}

static void CreatePrimitiveQueries(flecs::iter& it) {
  while (it.next()) {
    auto world = it.world();
    auto pq    = it.field<component::PrimitiveQuery>(1);

    for (auto i : it) {
      auto bq = world.query_builder()
                    .with<const component::Transform2d>()
                    .src(flecs::Self)
                    .with<const component::MaterialID>()
                    .src(flecs::Up)
                    .trav(flecs::IsA)
                    .optional()
                    .with<const component::PrimitiveQuery>()
                    .id(pq[i].component)
                    .with<component::Position2d>()
                    .src(flecs::Self)
                    .inout()
                    .cached()
                    .cache_kind(flecs::QueryCacheAuto);
    }
  }
}

static void PopulateBuffers(flecs::entity, const component::Primitive& p,
                            const component::PrimitiveQuery& q) {

  // populate_buffers();
}

PrimitiveSystem::PrimitiveSystem(flecs::world& world) {
  world.module<PrimitiveSystem>();

  world.import <component::PrimitiveComponent>();
  world.import <component::RendererComponent>();
  world.import <component::TransformComponent>();
  world.import <component::MaterialComponent>();

  world.entity<component::Rectangle2d>().emplace<component::PrimitiveQuery>(
      world.id<component::Rect2d>());

  world.observer<component::PrimitiveQuery>()
      .event(flecs::OnSet)
      .run(CreatePrimitiveQueries);

  world.system<const component::Primitive, const component::PrimitiveQuery>()
      .kind(flecs::PreStore)
      .each(PopulateBuffers);
}
} // namespace wen::system
