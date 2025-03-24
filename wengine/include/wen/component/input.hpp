#ifndef WEN_COMPONENT_INPUT_HPP_
#define WEN_COMPONENT_INPUT_HPP_

#include <flecs.h>
#include <SDL3/SDL.h>

#include "wen/wen_defines.hpp"

namespace wen::component {
struct WEN_API_EXPORT KeyStateComponent {
  bool is_pressed = false;
  bool state      = false;
  bool current    = false;

  KeyStateComponent() = default;
};

struct WEN_API_EXPORT MouseCoordComponent {
  float x = 0;
  float y = 0;

  MouseCoordComponent() = default;
};

struct WEN_API_EXPORT MouseStateComponent {
  KeyStateComponent left;
  KeyStateComponent right;
  KeyStateComponent wnd;
  KeyStateComponent rel;
  KeyStateComponent view;

  MouseStateComponent() = default;
};

struct WEN_API_EXPORT Input{
  KeyStateComponent   keys[256];
  MouseStateComponent mouse_state;

  Input() = default;
};

class WEN_API_EXPORT InputComponent {
public:
  explicit InputComponent(flecs::world& world);
};
} // namespace wen::component

#endif // WEN_COMPONENT_INPUT_HPP_
