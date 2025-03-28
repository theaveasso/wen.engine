#include "wen/system/wen_material.hpp"

#include "wen/component/wen_material.hpp"
#include <iostream>

namespace wen::system {
static void InitMaterials(flecs::iter& it) {
  std::cout << "init material" << "\n";
}

static void RegisterMaterial(flecs::iter& it) {
  std::cout << "RegisterMaterial" << "\n";
}

MaterialSystem::MaterialSystem(flecs::world& world) {
  world.module<MaterialSystem>();

  world.import <component::MaterialComponent>();

  auto init_mat_sys =
      world.system<const component::MaterialQuery, component::Materials>()
          .kind(flecs::OnLoad)
          .run(InitMaterials);

  init_mat_sys.add_second<component::Materials>(
      world.query_builder()
          .with<component::MaterialID>()
          .src(flecs::Self)
          // todo add specular term (optional)
          // todo add emissive term (optional)
          // trait prefab
          .cached()
          .cache_kind(flecs::QueryCacheAuto)
          .build()
          .entity()
          .set_name("#0.wen.system.materials.query"));

  world.system<component::MaterialID>()
      .without<component::MaterialID>()
      .src(flecs::Self)
      // todo add specular term (optional)
      // todo add emissive term (optional)
      // trait prefab
      .kind(flecs::PostLoad)
      .run(RegisterMaterial);
}
} // namespace wen::system
