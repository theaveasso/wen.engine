#ifndef WEN_EVENT_HPP_
#define WEN_EVENT_HPP_

#include <cstdint>
#include <flecs.h>

#include "wen/internal/wen_internal_defines.hpp"

typedef struct event_context_t {
  union {
    int64_t  i64[2];
    uint64_t u64[2];
    double   f64[2];

    int32_t  i32[4];
    uint32_t u32[4];
    float    f32[4];

    int16_t  i16[8];
    uint16_t u16[8];

    int8_t  i8[16];
    uint8_t u8[16];

    char c[16];
  } data; // 128 bytes
} event_context_t;

/**
 * System internal event code. Application should use codes beyond 255
 */
typedef enum system_event_code {
  // Shuts the application down on the next frame.
  EVENT_CODE_APPLICATION_QUIT = 0x01,

  /*
   * Keyboard key pressed
   * Context usage:
   * uint16_t key_code = data.data.u16[0]
   */
  EVENT_CODE_KEY_PRESSED = 0x02,

  /*
   * Keyboard key release
   * Context usage:
   * uint16_t key_code = data.data.u16[0]
   */
  EVENT_CODE_KEY_RELEASED = 0x03,

  /* Mouse button pressed
   * Context usage:
   * uint16_t key_code = data.data.u16[0]
   */
  EVENT_CODE_BUTTON_PRESSED = 0x04,

  /*
   * Mouse button released
   * Context usage:
   * uint16_t key_code = data.data.u16[0]
   */
  EVENT_CODE_BUTTON_RELEASED = 0x05,

  /*
   * Mouse moved.
   * Context usage:
   * uint16_t key_code = data.data.u16[0]
   */
  EVENT_CODE_MOUSE_MOVED = 0x06,

  /*
   * Mouse wheel.
   * Context usage:
   * uint16_t key_code = data.data.u16[0]
   */
  EVENT_CODE_MOUSE_WHEEL = 0x07,

  /*
   * Resized/resolution changed from the OS
   * Context usage:
   * uint16_t width = data.data.u16[0]
   * uint16_t height = data.data.u16[1]
   */
  EVENT_CODE_WINDOW_RESIZED = 0x08,

  EVENT_CODE_TOTAL_COUNT = 0xFF,
} system_event_code;

typedef bool(*on_event_proc_t)(uint16_t code, void* sender, void* listener,
                              event_context_t ctx);
/**
 * @return true if . otherwise false.
 */
bool event_system_initialize();

/**
 *
 */
void event_system_shutdown();

/**
 * @param code_
 * @param listener_
 * @param on_event_
 * @return true if the event is successfully registered, otherwise false.
 */
WEN_API bool event_register(uint16_t code, void* listener,
                            on_event_proc_t on_event);

/**
 *
 * @param code_
 * @param listener_
 * @param on_event_
 * @return true if the event is successfully unregistered, otherwise false.
 */
WEN_API bool event_unregister(uint16_t code_, void* listener_,
                              on_event_proc_t on_event_);

/**
 *
 * @param code_ The event code to fire.
 * @param sender_ A pointer to the sender. Can be 0/nullptr.
 * @param event_context_t The event data.
 * @return true if handled, otherwise false.
 */
WEN_API bool event_fire(uint16_t code_, void* sender_, event_context_t ctx_);

#endif // WEN_EVENT_HPP_
