#ifndef __WEN_SYSTEM_ENGINE_HPP_
#define __WEN_SYSTEM_ENGINE_HPP_

#include <flecs.h>

#include "wen/wen_defines.hpp"

namespace wen {
namespace system {
class WEN_API_EXPORT Engine {
public:
  explicit Engine(flecs::world& world);
};
} // namespace system
} // namespace wen

#endif // __WEN_SYSTEM_WINDOW_HPP_
