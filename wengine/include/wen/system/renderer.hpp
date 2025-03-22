#ifndef WEN_SYSTEM_RENDERER_HPP_
#define WEN_SYSTEM_RENDERER_HPP_

#include <flecs.h>

#include "wen/wen_defines.hpp"

namespace wen::system {
class WEN_API_EXPORT SDL_RendererSystem {
public:
  explicit SDL_RendererSystem(flecs::world& world);
};
} // namespace wen::system


#endif // WEN_SYSTEM_RENDERER_HPP_
