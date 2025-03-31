#ifndef RENDERER_VULKAN_VULKAN_BACKEND_HPP_
#define RENDERER_VULKAN_VULKAN_BACKEND_HPP_

#include "wen/renderer/wen_renderer_backend.hpp"

bool vulkan_renderer_backend_initialize(wen_renderer_backend_t* backend_, const char* name_, struct wen_platform_state_t* state_);
void vulkan_renderer_backend_shutdown(wen_renderer_backend_t* backend_);
void vulkan_renderer_backend_resize(wen_renderer_backend_t* backend_, uint16_t width_, uint16_t height_);

bool vulkan_renderer_backend_begin_frame(wen_renderer_backend_t* backend_, float deltatime_);
bool vulkan_renderer_backend_end_frame(wen_renderer_backend_t* backend_, float deltatime_);

#endif // RENDERER_VULKAN_VULKAN_BACKEND_HPP_
