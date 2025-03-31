#ifndef RENDERER_WEN_RENDERER_BACKEND_HPP_
#define RENDERER_WEN_RENDERER_BACKEND_HPP_

#include "wen_renderer_types.inl"

struct wen_platform_state_t;

bool renderer_backend_create(wen_renderer_backend_type_t type_, struct wen_platform_state_t* state_, wen_renderer_backend_t* out_renderer_backend_);
void renderer_backend_destroy(wen_renderer_backend_t* renderer_backend_);

#endif // RENDERER_WEN_RENDERER_BACKEND_HPP_
