#ifndef WEN_SYSTEM_WINDOW_HPP_
#define WEN_SYSTEM_WINDOW_HPP_

#include <flecs.h>

#include "wen/wen_defines.hpp"

namespace wen::system {
class WEN_API_EXPORT WindowSystem {
public:
  explicit WindowSystem(flecs::world& world);
};
} // namespace wen::system

#endif // WEN_SYSTEM_WINDOW_HPP_
