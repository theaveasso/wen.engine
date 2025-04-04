#pragma once

#include "vk_types.hpp"

void vk_context_init(WenVkContext *vk_ctx, const struct WenWindow *window, const char* app_name);
void vk_context_fini(WenVkContext *vk_ctx);
