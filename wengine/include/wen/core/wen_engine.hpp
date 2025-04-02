#ifndef WEN_CORE_WEN_ENGINE_HPP_
#define WEN_CORE_WEN_ENGINE_HPP_

#include "wen/wen.hpp"

#include <cstdint>

struct wen_game_t;

typedef struct wen_app_config_t {
  int16_t start_pos_x;
  int16_t start_pos_y;

  int16_t start_width;
  int16_t start_height;

  const char* title;
} wen_app_config_t;

/** Engine-related functions. */
WEN_API bool engine_create(struct wen_game_t* game_inst);
WEN_API bool engine_run();
WEN_API void engine_get_framebuffer_size(uint32_t* width, uint32_t* height);

#endif // WEN_CORE_WEN_ENGINE_HPP_
