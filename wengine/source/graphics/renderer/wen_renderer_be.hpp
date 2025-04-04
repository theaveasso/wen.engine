#pragma once

#include "wen_renderer_types.hpp"

bool renderer_backend_init(WenRendererType type, WenRendererBackend* out_renderer_backend);
bool renderer_backend_fini();
