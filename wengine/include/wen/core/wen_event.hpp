#ifndef WEN_EVENT_HPP_
#define WEN_EVENT_HPP_

#include <cstdint>
#include <flecs.h>

#include "wen/wen.hpp"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * @brief Event context data structure.
 * This structure stores contextual data for an event in a union of various data types.
 * The total size of the data block is 128 bits (16 bytes).
 */
typedef struct event_context_t {
  union {
    int64_t  i64[2]; /**< signed 64-bit integers */
    uint64_t u64[2]; /**< unsigned 64-bit integers */
    double   f64[2]; /**< double-precision floating points */

    int32_t  i32[4]; /**< signed 32-bit integers */
    uint32_t u32[4]; /**< unsigned 32-bit integers */
    float    f32[4]; /**< single-precision floating points */

    int16_t  i16[8]; /**< signed 16-bit integers */
    uint16_t u16[8]; /**< unsigned 16-bit integers */

    int8_t  i8[16]; /**< signed 8-bit integers */
    uint8_t u8[16]; /**< unsigned 8-bit integers */

    char c[16]; /**< character array (16 bytes) */

  } data; /**< 128 bytes */
} event_context_t;

/**
 * @brief System internal event code.
 * These codes represent system-level events.
 * Application should use codes beyond 255.
 */
typedef enum system_event_code {
  WEN_EVENT_CODE_APPLICATION_QUIT = 0x01, /**< Shuts the application down on the next frame. */
  WEN_EVENT_CODE_KEY_PRESSED      = 0x02, /**< Keyboard key pressed. */
  WEN_EVENT_CODE_KEY_RELEASED     = 0x03, /**< Keyboard key release. */
  WEN_EVENT_CODE_BUTTON_PRESSED   = 0x04, /**< Mouse button pressed. */
  WEN_EVENT_CODE_BUTTON_RELEASED  = 0x05, /**< Mouse button released. */
  WEN_EVENT_CODE_MOUSE_MOVED      = 0x06, /**< Mouse moved. */
  WEN_EVENT_CODE_MOUSE_WHEEL      = 0x07, /**< Mouse wheel. */
  WEN_EVENT_CODE_WINDOW_RESIZED   = 0x08, /**< Resized/resolution changed from the OS. */
  WEN_EVENT_CODE_WINDOW_MINIMIZED = 0x09, /**< Window minimized changed from the OS. */
  WEN_EVENT_CODE_WINDOW_MAXIMIZED = 0x10, /**< Window minimized changed from the OS. */

  WEN_EVENT_CODE_TOTAL_COUNT = 0xFF,
} system_event_code;

/**
 * @brief
 * @param uint16_t
 * @param void*
 * @param void*
 * @param event_context_t
 */
typedef bool (*on_event_proc_t)(uint16_t code, void* sender, void* listener, event_context_t ctx);

/**
 * @brief
 * @return true if . otherwise false.
 */
bool event_system_initialize();

/**
 * @brief
 */
void event_system_shutdown();

/**
 * @brief
 * @param code_
 * @param listener_
 * @param on_event_
 * @return true if the event is successfully registered, otherwise false.
 */
WEN_API bool event_register(uint16_t code, void* listener, on_event_proc_t on_event);

/**
 * @brief
 * @param uint16_t
 * @param void*
 * @param on_event_proc_t
 * @return true if the event is successfully unregistered, otherwise false.
 */
WEN_API bool event_unregister(uint16_t code_, void* listener_, on_event_proc_t on_event_);

/**
 * @brief
 * @param uint16_t
 * @param void*
 * @param on_event_proc_t
 * @return true if handled, otherwise false.
 */
WEN_API bool event_fire(uint16_t code_, void* sender_, event_context_t ctx_);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // WEN_EVENT_HPP_
