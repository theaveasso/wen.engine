#include "wen/core/wen_event.hpp"

#include "wen/core/wen_memory.hpp"
#include "wen/datastructures/wen_vec.hpp"
#include "wen/internal/wen_internal_defines.hpp"
#include "wen/wen_defines.hpp"

/**
 *
 */
typedef struct registered_event_t {
  void*           listener;
  on_event_proc_t callback;
} registered_event_t;

/**
 *
 */
typedef struct event_code_entry_t {
  registered_event_t* events;
} event_code_entry_t;

/**
 *
 */
typedef struct event_system_state_t {
  event_code_entry_t registered[WEN_MESSAGE_CODE_COUNT_MAX];
} event_system_state_t;

/**
 *
 */
static bool                 is_initialized = false;
static event_system_state_t event_state;

bool event_system_initialize() {
  if (is_initialized) {
    return false;
  }
  is_initialized = false;
  wen_memzero(&event_state, sizeof(event_state));
  is_initialized = true;

  return true;
}

void event_system_shutdown() {
  for (auto& i : event_state.registered) {
    if (i.events != nullptr) {
      vec_destroy(i.events);
      i.events = nullptr;
    }
  }
}

bool event_register(uint16_t code, void* listener, on_event_proc_t on_event) {
  /** event system not initialize. */
  if (!is_initialized) {
    return false;
  }

  /** register new event type. */
  if (event_state.registered[code].events == nullptr) {
    event_state.registered[code].events = vec_create(registered_event_t);
  }

  /** filter out the event from the same listener. */
  uint64_t registered_count = vec_len(event_state.registered[code].events);
  for (uint64_t i = 0; i < registered_count; ++i) {
    registered_event_t e = event_state.registered[code].events[i];
    if (e.listener == listener) {
      // SAME LISTENER
      // TODO WARN
      return false;
    }
  }

  /** push new registered event into event queue. */
  registered_event_t event;
  event.listener = listener;
  event.callback = on_event;
  vec_push(event_state.registered[code].events, event);

  return true;
}

bool event_unregister(uint16_t code_, void* listener_,
                      on_event_proc_t on_event_) {
  /** event system not initialize. */
  if (!is_initialized) {
    return false;
  }

  /** event registered not found in events code. */
  if (!event_state.registered[code_].events) {
    return false;
  }

  /** iterate through event code to look up for registered event. */
  uint64_t registered_count = vec_len(event_state.registered[code_].events);
  for (uint64_t i = 0; i < registered_count; i++) {
    registered_event_t e = event_state.registered[code_].events[i];
    if (e.listener == listener_ && e.callback == on_event_) {
      // EVENT FOUND
      // TODO WARN
      registered_event_t popped_event;
      vec_pop_at(event_state.registered[code_].events, i, &popped_event);
      return true;
    }
  }

  /** event not found. */
  return false;
}

bool event_fire(uint16_t code_, void* sender_, event_context_t ctx_) {
  /** event system not initialize. */
  if (!is_initialized) {
    return false;
  }

  /** event registered not found in events code. */
  if (!event_state.registered[code_].events) {
    return false;
  }

  uint64_t registered_count = vec_len(event_state.registered[code_].events);
  for (uint64_t i = 0; i < registered_count; ++i) {
    registered_event_t e = event_state.registered[code_].events[i];
    if (e.callback(code_, sender_, e.listener, ctx_)) {
      return true;
    }
  }

  /** event not found. */
  return false;
}
