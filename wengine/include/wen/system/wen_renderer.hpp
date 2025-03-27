#ifndef WEN_SYSTEM_WEN_RENDERER_HPP_
#define WEN_SYSTEM_WEN_RENDERER_HPP_

#include <flecs.h>

namespace wen::system {
class RendererSystem {
public:
  explicit RendererSystem(flecs::world& world);
};
} // namespace wen::system

#endif // WEN_SYSTEM_WEN_RENDERER_HPP_
