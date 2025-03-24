#ifndef WEN_SYSTEM_GUI_HPP_
#define WEN_SYSTEM_GUI_HPP_

#include <flecs.h>

#include "wen/wen_defines.hpp"

namespace wen::system {
class WEN_API_EXPORT GUISystem{
public:
  explicit GUISystem(flecs::world& world);
};
} // namespace wen::system


#endif // WEN_SYSTEM_RENDERER_HPP_
