#ifndef WEN_SYSTEM_WEN_PRIMITIVE_HPP_
#define WEN_SYSTEM_WEN_PRIMITIVE_HPP_

#include <flecs.h>

namespace wen::system {

class PrimitiveSystem {
public:
  explicit PrimitiveSystem(flecs::world& world);
};
} // namespace wen::system

#endif // WEN_SYSTEM_WEN_PRIMITIVE_HPP_
