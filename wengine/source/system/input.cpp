#include <SDL3/SDL.h>
#include <iostream>

#include "wen/system/input.hpp"

#include "wen/component/input.hpp"

namespace wen::system {
static void OnKeyUp(component::KeyStateComponent& key_state) {
}

static void OnKeyDown(component::KeyStateComponent& key_state) {
}

static void OnKeyReset(component::KeyStateComponent& key_state) {
  if (key_state.current) {
    key_state.state      = false;
    key_state.is_pressed = false;
  } else if (key_state.state) {
    key_state.is_pressed = false;
  }
}

void ProcessEvent(flecs::iter& it, size_t i, component::InputComponent& comp) {
  for (int k = 0; k < 256; k++) {
    component::KeyStateComponent key = comp.keys[k];
    OnKeyReset(key);
  }
  OnKeyReset(comp.mouse_state.left);
  OnKeyReset(comp.mouse_state.right);

  SDL_Event event{};
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_EVENT_QUIT: it.world().quit(); break;
    case SDL_EVENT_KEY_DOWN: std::cout << "key down" << "\n"; break;
    case SDL_EVENT_KEY_UP: std::cout << "key Up" << "\n"; break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
      switch (event.button.button) {
      case SDL_BUTTON_LEFT: std::cout << "mouse left down" << "\n"; break;
      case SDL_BUTTON_RIGHT: std::cout << "mouse right down" << "\n"; break;
      default: break;
      }
      break;
    case SDL_EVENT_MOUSE_BUTTON_UP:
      switch (event.button.button) {
      case SDL_BUTTON_LEFT: std::cout << "mouse left up" << "\n"; break;
      case SDL_BUTTON_RIGHT: std::cout << "mouse right up" << "\n"; break;
      default: break;
      }
      break;
    default: break;
    }
  }
}

} // namespace wen::system
