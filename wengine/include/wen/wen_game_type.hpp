#ifndef WEN_GAME_TYPE_HPP_
#define WEN_GAME_TYPE_HPP_

#include "wen_engine.hpp"

typedef struct game_t {
  app_config_t app_config;

  bool (*initialize)(struct game_t* game_inst);

  bool (*update)(struct game_t* game_inst, float dt);

  bool (*render)(struct game_t* game_inst, float dt);

  void (*on_resize)(struct game_t* game_inst, uint32_t width, uint32_t height);

  void* state;
} game_t;

#endif // WEN_GAME_TYPE_HPP_
