#ifndef WEN_SYSTEM_WEN_INPUT_HPP_
#define WEN_SYSTEM_WEN_INPUT_HPP_

#include <flecs.h>

namespace wen::system {
class InputSystem {
public:
  explicit InputSystem(flecs::world& world);
};
} // namespace wen::system

#endif // WEN_SYSTEM_WENINPUT_HPP_
