#include "wen/system/transform.hpp"

#include "wen/component/transform.hpp"

namespace wen::system {
static void AddTransform(flecs::entity                      e,
                         component::transform::Transform2d& t) {};

static void ApplyTransform(flecs::entity                      e,
                           component::transform::Transform2d& t) {};

Transform::Transform(flecs::world& world) {
  world.module<Transform>();

  world.import <component::Transform>();

  world.system<component::transform::Transform2d>()
      .kind(flecs::PostLoad)
      .each(AddTransform);

  world.system<component::transform::Transform2d>()
      .kind(flecs::OnValidate)
      .each(ApplyTransform);
}
} // namespace wen::system
