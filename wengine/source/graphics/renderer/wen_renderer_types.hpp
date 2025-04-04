#pragma once

#include <cstdint>
#include "core/wen_core_options.hpp"

struct WenRendererBackend
{
  uint16_t         frame_count;
  struct WenWindow *platform;

  bool (*init)(struct WenRendererBackend *backend, struct WenWindow* window, const char* app_name, int32_t width, int32_t height);
  void (*fini)(struct WenRendererBackend *backend);
  void (*resize)(struct WenRendererBackend *backend, uint32_t width, uint32_t height);
  bool (*begin_frame)(struct WenRendererBackend *backend, float dt);
  bool (*end_frame)(struct WenRendererBackend *backend, float dt);
};

struct WenRenderer
{
  WenRendererBackend *backend;
  WenRendererType    type;
};
