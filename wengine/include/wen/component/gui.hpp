#ifndef __WEN_COMPONENT_GUI_HPP_
#define __WEN_COMPONENT_GUI_HPP_

#include "wen/wen_defines.hpp"

#include <flecs.h>
#include <string>

namespace wen {
namespace component {

struct WEN_API_EXPORT GUIComponent {
  std::string title            = "WEN";
  int32_t     width            = 1280;
  int32_t     height           = 720;
  Color       background_color = {255, 255, 255, 255};

  GUIComponent() = default;
};

class WEN_API_EXPORT GUI {
public:
  GUI(flecs::world& world);
};
} // namespace component
} // namespace wen

#endif // __WEN_COMPONENT_GUI_HPP_
