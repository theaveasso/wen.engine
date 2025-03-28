#ifndef WEN_COMPONENT_WEN_PRIMITIVE_HPP_
#define WEN_COMPONENT_WEN_PRIMITIVE_HPP_

#include <SDL3/SDL.h>
#include <flecs.h>
#include <vector>

namespace wen::component {
struct Rect2d {
  float x, y;
  float w, h;
};

struct Box3d {};

struct PrimitiveBuffers {
  std::vector<float> colors_data;
  std::vector<float> transforms_data;
  std::vector<float> materials_data;

  SDL_GPUBuffer* colors;
  SDL_GPUBuffer* transforms;
  SDL_GPUBuffer* materials;

  int instance_count;
};

struct Primitive {
  SDL_GPUBuffer* vertices;
  SDL_GPUBuffer* normals;
  SDL_GPUBuffer* indices;

  int index_count;

  PrimitiveBuffers solid;
  PrimitiveBuffers emissive;

  // allocator
  // callback action (populate)
};

struct PrimitiveQuery {
  flecs::entity_t component;

  flecs::entity_t parent_query;
  flecs::entity_t solid;
  flecs::entity_t emissive;

  explicit PrimitiveQuery(flecs::entity_t e_) : component(e_) {}
};

class PrimitiveComponent {
public:
  explicit PrimitiveComponent(flecs::world& world);
};
} // namespace wen::component

#endif // WEN_COMPONENT_WEN_PRIMITIVE_HPP_
