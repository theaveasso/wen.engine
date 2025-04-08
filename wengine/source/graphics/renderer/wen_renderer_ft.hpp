#pragma once

#include "wen_renderer_types.hpp"

void renderer_init(WenRendererType type, WenRenderer *renderer, WenWindow &window, const char *app_name, int32_t width, int32_t height);
void renderer_fini(WenRenderer *renderer);

bool renderer_begin_frame(WenRenderer *renderer);
bool renderer_end_frame(WenRenderer *renderer);
void renderer_draw_frame(WenRenderer *renderer);