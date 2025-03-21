#ifndef __WEN_SYSTEM_INPUT_HPP_
#define __WEN_SYSTEM_INPUT_HPP_

#include <flecs.h>

#include "wen/component/input.hpp"

namespace wen {
namespace system {
void ProcessEvent(flecs::iter& it, size_t i, component::InputComponent& comp);
} // namespace system
} // namespace wen

#endif // __WEN_SYSTEM_INPUT_HPP_
