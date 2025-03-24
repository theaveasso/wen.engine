#ifndef WEN_SYSTEM_TRANSFORM_HPP_
#define WEN_SYSTEM_TRANSFORM_HPP_

#include <flecs.h>

#include "wen/wen_defines.hpp"

namespace wen::system {
class WEN_API_EXPORT Transform {
  explicit Transform(flecs::world& world);

};
} // namespace wen::system

#endif // WEN_SYSTEM_TRANSFORM_HPP_
