/**
 * @file wen_clock.hpp
 * @brief A clock system for tracking elapsed time.
 */
#ifndef WEN_RENDERER_RENDERER_TYPES_INL_
#define WEN_RENDERER_RENDERER_TYPES_INL_

#include <cstdint>

typedef enum wen_renderer_backend_type_t {
  RENDERER_BACKEND_TYPE_SDL3,
  RENDERER_BACKEND_TYPE_VULKAN,
  RENDERER_BACKEND_TYPE_OPENGL,
  RENDERER_BACKEND_TYPE_DIRECTX,
} wen_renderer_backend_type_t;

typedef struct wen_render_packet_t {
  float deltatime;
} wen_render_packet_t;

typedef struct wen_renderer_backend_t {
  uint64_t frame_count;
  struct wen_platform_state_t* platform_state;
  bool (*initialize)(struct wen_renderer_backend_t* backend_, const char* name_, struct wen_platform_state_t* state_);
  void (*shutdown)(struct wen_renderer_backend_t* backend_);
  void (*resize)(struct wen_renderer_backend_t* backend_, uint16_t width_, uint16_t height_);

  bool (*begin_frame)(struct wen_renderer_backend_t* backend_, float deltatime_);
  bool (*end_frame)(struct wen_renderer_backend_t* backend_, float deltatime_);
} wen_renderer_backend_t;

#endif // WEN_RENDERER_RENDERER_TYPES_INL_
