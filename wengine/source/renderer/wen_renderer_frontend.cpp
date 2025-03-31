#include "wen/renderer/wen_renderer_frontend.hpp"
#include "wen/renderer/wen_renderer_backend.hpp"

#include "wen/core/wen_logger.hpp"
#include "wen/core/wen_memory.hpp"

static wen_renderer_backend_t* backend = nullptr;

bool renderer_initialize(const char* name_, struct wen_platform_state_t* state_) {
  backend = (wen_renderer_backend_t*)wen_memalloc(sizeof(wen_renderer_backend_t), MEMORY_TAG_RENDERER);

  renderer_backend_create(RENDERER_BACKEND_TYPE_VULKAN, state_, backend);
  if (!backend->initialize(backend, name_, state_)) {
    return false;
  }
  backend->frame_count = 0;

  return true;
}

void renderer_shutdown() {
  backend->shutdown(backend);
  wen_memfree(backend, sizeof(wen_renderer_backend_t), MEMORY_TAG_RENDERER);
}

void renderer_on_resized(uint16_t width_, uint16_t height_) {
}

bool renderer_begin_frame(float deltatime) {
  return backend->begin_frame(backend, deltatime);
}

bool renderer_end_frame(float deltatime) {
  bool result = backend->end_frame(backend, deltatime);
  backend->frame_count++;
  return result;
}

bool renderer_draw_frame(wen_render_packet_t* packet) {
  bool bf_result = renderer_begin_frame(packet->deltatime);
  if (!bf_result) {
    return true;
  }

  bool ef_result = backend->end_frame(backend, packet->deltatime);
  if (!ef_result) {
    return false;
  }

  return true;
}
