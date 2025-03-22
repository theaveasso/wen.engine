#ifndef WEN_SYSTEM_INPUT_HPP_
#define WEN_SYSTEM_INPUT_HPP_

#include <flecs.h>

#include "wen/component/input.hpp"

namespace wen::system {
void ProcessEvent(flecs::iter& it, size_t i, component::InputComponent& comp);
} // namespace wen::system


#endif // WEN_SYSTEM_INPUT_HPP_
