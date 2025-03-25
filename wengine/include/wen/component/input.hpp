#ifndef WEN_COMPONENT_INPUT_HPP_
#define WEN_COMPONENT_INPUT_HPP_

#include <SDL3/SDL.h>
#include <flecs.h>

#include "wen/wen_defines.hpp"

namespace wen::component {
struct WEN_API_EXPORT KeyStateComponent {
  bool is_pressed = false;
  bool state      = false;
  bool current    = false;
};

struct WEN_API_EXPORT MouseCoordComponent {
  float x = 0;
  float y = 0;
};

struct WEN_API_EXPORT MouseStateComponent {
  KeyStateComponent left;
  KeyStateComponent right;
  KeyStateComponent wnd;
  KeyStateComponent rel;
  KeyStateComponent view;
};

struct WEN_API_EXPORT Input {
  KeyStateComponent   keys[SDL_SCANCODE_COUNT];
  MouseStateComponent mouse_state;
};

struct WEN_API_EXPORT InputMapping {
  flecs::entity_t action;
  int             key;
};

class WEN_API_EXPORT InputComponent {
public:
  explicit InputComponent(flecs::world& world);
};
} // namespace wen::component

#endif // WEN_COMPONENT_INPUT_HPP_
