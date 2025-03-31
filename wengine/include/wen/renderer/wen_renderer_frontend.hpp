#ifndef RENDERER_WEN_RENDERER_FRONTEND_HPP_
#define RENDERER_WEN_RENDERER_FRONTEND_HPP_

#include "wen_renderer_types.inl"

struct wen_static_mesh_data_t;
struct wen_platform_state_t;

bool renderer_initialize(const char* name_, struct wen_platform_state_t* state_);
void renderer_shutdown();

void renderer_on_resized(uint16_t width_, uint16_t height_);

bool renderer_begin_frame(float deltatime);
bool renderer_draw_frame(wen_render_packet_t* packet);
bool renderer_end_frame(float deltatime);

#endif // RENDERER_WEN_RENDERER_FRONTEND_HPP_
