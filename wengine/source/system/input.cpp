#include "wen/system/input.hpp"

#include <iostream>

#include "wen/component/input.hpp"

namespace wen::system {
static void OnKeyUp(component::KeyStateComponent& key_state) {
  key_state.current = false;
}

static void OnKeyDown(component::KeyStateComponent& key_state) {
  if (key_state.state) {
    key_state.is_pressed = false;
  } else {
    key_state.is_pressed = true;
  }

  key_state.state   = true;
  key_state.current = true;
}

static void OnKeyReset(component::KeyStateComponent& key_state) {
  if (key_state.current) {
    key_state.state      = false;
    key_state.is_pressed = false;
  } else if (key_state.state) {
    key_state.is_pressed = false;
  }
}

void ProcessEvent(flecs::iter& it, size_t i, component::Input& comp) {
  for (auto key : comp.keys) {
    OnKeyReset(key);
  }
  OnKeyReset(comp.mouse_state.left);
  OnKeyReset(comp.mouse_state.right);

  SDL_Event event{};
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_EVENT_QUIT: it.world().quit(); break;
    case SDL_EVENT_KEY_DOWN: OnKeyDown(comp.keys[event.key.key]); break;
    case SDL_EVENT_KEY_UP: OnKeyUp(comp.keys[event.key.key]); break;
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

void InputMapping(flecs::iter it, size_t i, component::Input& input_comp,
                  component::InputMapping& input_mapping_comp) {
}

} // namespace wen::system
