#ifndef WEN_ENGINE_HPP_
#define WEN_ENGINE_HPP_

#include <cstdint>

#include "wen/internal/wen_internal_defines.hpp"

struct game_t;

typedef struct app_config_t {
  int16_t start_pos_x;
  int16_t start_pos_y;

  int16_t start_width;
  int16_t start_height;

  const char* title;
} app_config_t;

WEN_API bool engine_create(struct game_t* game_inst);
WEN_API bool engine_run();

#endif // WEN_ENGINE_HPP_
