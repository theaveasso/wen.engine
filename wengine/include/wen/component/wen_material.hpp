#ifndef WEN_COMPONENT_WEN_MATERIAL_HPP_
#define WEN_COMPONENT_WEN_MATERIAL_HPP_

#include <SDL3/SDL.h>
#include <flecs.h>

#include <array>

namespace wen::component {
#define MATERIALS_LIMIT (255)
struct Specular {
  float specular_power;
  float shiniess;
};

struct Emissive {
  float value;
};

struct MaterialID {
  uint32_t material_id;
};

struct Material {
  float specular_power;
  float shiniess;
  float emissive;
};

struct Materials {
  bool                                  has_updated;
  std::array<Material, MATERIALS_LIMIT> array;
};

struct MaterialQuery {
  flecs::query_t q;
};

class MaterialComponent {
public:
  explicit MaterialComponent(flecs::world& world);
};
} // namespace wen::component

#endif // WEN_COMPONENT_WEN_MATERIAL_HPP_
