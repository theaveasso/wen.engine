#include "game.hpp"

#include <wen/core/wen_memory.hpp>
#include <wen/wen_entry.hpp>

bool game_create(game_t* out_game) {
  out_game->app_config.start_pos_x  = 100;
  out_game->app_config.start_pos_y  = 100;
  out_game->app_config.start_width  = 100;
  out_game->app_config.start_height = 100;
  out_game->app_config.title        = "WEN ENGINE Sandbox";

  out_game->initialize = initialize;
  out_game->update     = on_update;
  out_game->render     = on_render;
  out_game->on_resize  = on_resize;

  game_state_t game_state;

  out_game->state =
      wen_memalloc(sizeof(game_state), MEMTAG_APPLICATION);

  return true;
}