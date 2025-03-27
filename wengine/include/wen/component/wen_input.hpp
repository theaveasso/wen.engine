#ifndef WEN_COMPONENT_INPUT_HPP_
#define WEN_COMPONENT_INPUT_HPP_

#include <SDL3/SDL.h>
#include <flecs.h>

#include "wen/wen_defines.hpp"

namespace wen::component {
struct KeyState {
  bool is_pressed = false;
  bool state      = false;
  bool current    = false;
};

struct MouseCoord {
  float x = 0;
  float y = 0;
};

struct MouseState {
  KeyState left;
  KeyState right;
  KeyState wnd;
  KeyState rel;
  KeyState view;
};

struct WEN_API_EXPORT Input {
  KeyState   keys[SDL_SCANCODE_COUNT];
  MouseState mouse_state;
};

class InputComponent {
public:
  explicit InputComponent(flecs::world& world);
};
} // namespace wen::component

#endif // WEN_COMPONENT_INPUT_HPP_
