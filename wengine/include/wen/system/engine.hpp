#ifndef WEN_SYSTEM_ENGINE_HPP_
#define WEN_SYSTEM_ENGINE_HPP_

#include <flecs.h>

#include "wen/wen_defines.hpp"

namespace wen::system {
class WEN_API_EXPORT Engine {
public:
  explicit Engine(flecs::world& world);
};
} // namespace wen::system

#endif // WEN_SYSTEM_ENGINE_HPP_
