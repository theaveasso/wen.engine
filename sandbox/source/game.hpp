#ifndef GAME_HPP_
#define GAME_HPP_

#include <wen/wen_game_type.hpp>

typedef struct game_state_t {
  float dt;
} game_state_t;

bool initialize(game_t* game_inst);

bool on_update(game_t* game_inst, float dt);

bool on_render(game_t* game_inst, float dt);

void on_resize(game_t* game_inst, uint32_t width, uint32_t height);

#endif // GAME_HPP_
