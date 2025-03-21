#ifndef __WEN_SYSTEM_RENDERER_HPP_
#define __WEN_SYSTEM_RENDERER_HPP_

#include <flecs.h>

#include "wen/wen_defines.hpp"

namespace wen {
namespace system {
class WEN_API_EXPORT SDL_RendererSystem {
public:
  explicit SDL_RendererSystem(flecs::world& world);
};
} // namespace system
} // namespace wen

#endif // __WEN_SYSTEM_RENDERER_HPP_
