#ifndef WEN_GAME_TYPE_HPP_
#define WEN_GAME_TYPE_HPP_

#include "wen/core/wen_engine.hpp"
#include <cstdint>

/**
 * Represents the basic game state in a game.
 * Called for creation by the application.
 */
typedef struct wen_game_t {
  wen_app_config_t app_config;                                                      /**< The application configuration. */
  bool (*initialize)(struct wen_game_t* game_inst);                                 /**< Proc to game's initialize function. */
  bool (*update)(struct wen_game_t* game_inst, float dt);                           /**< Proc to game' update function. */
  bool (*render)(struct wen_game_t* game_inst, float dt);                           /**< Proc to game' render function. */
  void (*on_resize)(struct wen_game_t* game_inst, uint32_t width, uint32_t height); /**< Proc to handle resizes, if applicable */
  void* state;                                                                      /** Game-specific game state. Created and managed by the game. */
} wen_game_t;

#endif // WEN_GAME_TYPE_HPP_
