#include "wen/renderer/wen_renderer_backend.hpp"

#include "wen/renderer/vulkan/vulkan_backend.hpp"

bool renderer_backend_create(wen_renderer_backend_type_t type_, struct wen_platform_state_t* state_, wen_renderer_backend_t* out_renderer_backend_) {
  out_renderer_backend_->platform_state = state_;
  if (type_ == RENDERER_BACKEND_TYPE_VULKAN) {
    out_renderer_backend_->initialize = vulkan_renderer_backend_initialize;
    out_renderer_backend_->shutdown = vulkan_renderer_backend_shutdown;
    out_renderer_backend_->resize = vulkan_renderer_backend_resize;
    out_renderer_backend_->begin_frame = vulkan_renderer_backend_begin_frame;
    out_renderer_backend_->end_frame = vulkan_renderer_backend_end_frame;
    return true;
  } else if (type_ == RENDERER_BACKEND_TYPE_SDL3) {
    return true;
  }
  return false;
}

void renderer_backend_destroy(wen_renderer_backend_t* renderer_backend_) {
  renderer_backend_->initialize  = nullptr;
  renderer_backend_->shutdown    = nullptr;
  renderer_backend_->begin_frame = nullptr;
  renderer_backend_->end_frame   = nullptr;
  renderer_backend_->resize      = nullptr;
}
