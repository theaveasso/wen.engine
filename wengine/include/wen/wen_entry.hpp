#ifndef WEN_ENTRY_HPP_
#define WEN_ENTRY_HPP_

#include "wen/core/wen_logger.hpp"
#include "wen/core/wen_memory.hpp"
#include "wen_defines.hpp"
#include "wen_engine.hpp"
#include "wen_game_type.hpp"

extern bool game_create(game_t* out_game);

/**
 * The main entry point of the application.
 */
int main(int, char**) {
  wen_meminit();

  game_t game_inst{};
  if (!game_create(&game_inst)) {
    return -1;
  }
  if (!game_inst.initialize || !game_inst.update || !game_inst.render ||
      !game_inst.on_resize) {
    return -2;
  }

  if (!engine_create(&game_inst)) {
    return 1;
  }
  if (!engine_run()) {
    return 2;
  }

  wen_memcleanup();

  return 0;
}

#endif // WEN_ENTRY_HPP_
