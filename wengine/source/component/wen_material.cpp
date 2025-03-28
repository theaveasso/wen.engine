#include "wen/component/wen_material.hpp"
#include "flecs/private/addons.h"

namespace wen::component {
MaterialComponent::MaterialComponent(flecs::world& world) {
  world.module<MaterialComponent>();

  world.component<MaterialQuery>();
  world.component<MaterialID>().add(flecs::OnInstantiate, flecs::Inherit);
  world.component<Materials>();
}
} // namespace wen::component
