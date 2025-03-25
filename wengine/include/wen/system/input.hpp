#ifndef WEN_SYSTEM_INPUT_HPP_
#define WEN_SYSTEM_INPUT_HPP_

#include <flecs.h>

#include "wen/component/input.hpp"

namespace wen::system {
void ProcessEvent(flecs::iter& it, size_t i, component::Input& comp);
void InputMapping(flecs::iter& it, size_t i, component::Input& input_comp,
                  component::InputMapping& input_mapping_comp);
} // namespace wen::system
#endif // WEN_SYSTEM_INPUT_HPP_
