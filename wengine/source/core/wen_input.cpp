#include "wen/core/wen_input.hpp"

#include "wen/core/wen_event.hpp"
#include "wen/core/wen_memory.hpp"

typedef struct keyboard_state_t {
  bool keys[256];
} keyboard_state_t;

typedef struct mouse_state_t {
  int16_t x;
  int16_t y;

  uint8_t buttons[BUTTON_COUNT];
} mouse_state_t;

typedef struct input_system_state_t {
  keyboard_state_t keyboard_current;  /**< Current frame keyboard state data. */
  keyboard_state_t keyboard_previous; /**< Previous frame keyboard state data. */
  mouse_state_t    mouse_current;     /**< Current frame mouse state data. */
  mouse_state_t    mouse_previous;    /**< Previous frame mouse state data. */
} input_system_state_t;

static bool                 is_initialized = false;
static input_system_state_t input_state;

void input_system_initialize() {
  is_initialized = true;
  wen_memzero(&input_state, sizeof(input_system_state_t));
}

void input_system_shutdown() {
  is_initialized = false;
}

void input_system_update(float dt) {
  if (!is_initialized) {
    return;
  }

  wen_memcpy(&input_state.keyboard_previous, &input_state.keyboard_current, sizeof(keyboard_state_t));
  wen_memcpy(&input_state.mouse_previous, &input_state.mouse_current, sizeof(mouse_state_t));
}

void input_process_key(keys_t key, bool is_pressed) {
  /** Only handle this if the state actually changed. */
  if (input_state.keyboard_current.keys[key] != is_pressed) {
    /** Update internal state. */
    input_state.keyboard_current.keys[key] = is_pressed;

    /** Fire off an event for immediate processing. */
    event_context_t ctx;
    ctx.data.u32[0] = key;
    event_fire(is_pressed
                   ? WEN_EVENT_CODE_KEY_PRESSED
                   : WEN_EVENT_CODE_KEY_RELEASED,
               nullptr, ctx);
  }
}

void input_process_button(buttons_t button_, bool is_pressed_) {
  /** Only handle this if the state actually changed. */
  if (input_state.mouse_current.buttons[button_] != is_pressed_) {
    /** Update internal state. */
    input_state.mouse_current.buttons[button_] = is_pressed_;

    /** Fire off an event for immediate processing. */
    event_context_t ctx;
    ctx.data.u16[0] = button_;
    event_fire(is_pressed_
                   ? WEN_EVENT_CODE_BUTTON_PRESSED
                   : WEN_EVENT_CODE_BUTTON_RELEASED,
               nullptr, ctx);
  }
}

void input_process_mouse_move(int16_t x_, int16_t y_) {
  /** Only handle this if the state actually changed. */
  if (input_state.mouse_current.x != x_ || input_state.mouse_current.y != y_) {
#if defined(WEN_DEBUG)
    // SDL_Log("mouse position: %i, %i!", x_, y_);
#endif

    /** Update internal state. */
    input_state.mouse_current.x = x_;
    input_state.mouse_current.y = y_;

    /** Fire off an event for immediate processing. */
    event_context_t ctx;
    ctx.data.u16[0] = x_;
    ctx.data.u16[1] = x_;
    event_fire(WEN_EVENT_CODE_MOUSE_MOVED, nullptr, ctx);
  }
}

void input_process_mouse_wheel(int8_t z_delta_) {
  if (!is_initialized) {
    return;
  }
  /** Fire off an event for immediate processing. */
  event_context_t ctx;
  ctx.data.u8[0] = z_delta_;
  event_fire(WEN_EVENT_CODE_MOUSE_WHEEL, nullptr, ctx);
}

bool wen_input_is_key_down(keys_t key_code_) {
  if (!is_initialized) {
    return false;
  }
  return input_state.keyboard_current.keys[key_code_];
}

bool wen_input_is_key_up(keys_t key_code_) {
  if (!is_initialized) {
    return true;
  }
  return !input_state.keyboard_current.keys[key_code_];
}

bool wen_input_was_key_down(keys_t key_code_) {
  if (!is_initialized) {
    return false;
  }
  return input_state.keyboard_previous.keys[key_code_];
}

bool wen_input_was_key_up(keys_t key_code_) {
  if (!is_initialized) {
    return true;
  }
  return !input_state.keyboard_previous.keys[key_code_];
}

bool wen_input_is_button_down(buttons_t button_) {
  if (!is_initialized) {
    return false;
  }
  return input_state.mouse_current.buttons[button_];
}

bool wen_input_is_button_up(buttons_t button_) {
  if (!is_initialized) {
    return true;
  }
  return !input_state.mouse_current.buttons[button_];
}

bool wen_input_was_button_down(buttons_t button_) {
  if (!is_initialized) {
    return false;
  }
  return input_state.mouse_previous.buttons[button_];
}

bool wen_input_was_button_up(buttons_t button_) {
  if (!is_initialized) {
    return true;
  }
  return !input_state.mouse_previous.buttons[button_];
}

void wen_input_get_mouse_pos(int32_t* x, int32_t* y) {
  if (!is_initialized) {
    *x = 0;
    *y = 0;
    return;
  }
  *x = input_state.mouse_current.x;
  *y = input_state.mouse_current.y;
}

void wen_input_get_prev_mouse_pos(int32_t* x, int32_t* y) {
  if (!is_initialized) {
    *x = 0;
    *y = 0;
    return;
  }
  *x = input_state.mouse_previous.x;
  *y = input_state.mouse_previous.y;
}
