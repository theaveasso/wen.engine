#include "wen/system/wen_input.hpp"

#include <iostream>

#include "wen/component/wen_input.hpp"
#include "wen/component/wen_pipeline_phase.hpp"

namespace wen::system {
static void OnKeyUp(component::KeyState& key_state) {
  key_state.current = false;
}

static void OnKeyDown(component::KeyState& key_state) {
  if (key_state.state) {
    key_state.is_pressed = false;
  } else {
    key_state.is_pressed = true;
  }

  key_state.state   = true;
  key_state.current = true;
}

static void OnKeyReset(component::KeyState& key_state) {
  if (key_state.current) {
    key_state.state      = false;
    key_state.is_pressed = false;
  } else if (key_state.state) {
    key_state.is_pressed = false;
  }
}

static void ProcessEvent(flecs::iter& it) {
  while (it.next()) {
    auto input = it.field<component::Input>(0);
    for (auto key : input->keys) {
      OnKeyReset(key);
    }
    OnKeyReset(input->mouse_state.left);
    OnKeyReset(input->mouse_state.right);

    SDL_Event event{};
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_EVENT_QUIT: it.world().quit(); break;
      case SDL_EVENT_KEY_DOWN: OnKeyDown(input->keys[event.key.key]); break;
      case SDL_EVENT_KEY_UP: OnKeyUp(input->keys[event.key.key]); break;
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
}

InputSystem::InputSystem(flecs::world& world) {
  world.module<InputSystem>();

  world.import <component::InputComponent>();
  world.import <component::PipelinePhaseComponent>();

  auto on_process_event_phase = world.entity<component::OnProcessEvent>();

  world.system<component::Input>()
      .kind(on_process_event_phase)
      .run(ProcessEvent);
}
} // namespace wen::system
