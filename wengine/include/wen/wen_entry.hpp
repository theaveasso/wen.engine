/**
 * @file
 * @brief
 */
#ifndef WEN_ENTRY_HPP_
#define WEN_ENTRY_HPP_

#include "wen.hpp"
#include "wen/core/wen_logger.hpp"
#include "wen/core/wen_memory.hpp"
#include "wen_game_type.hpp"

extern bool game_create(wen_game_t* out_game);

/**
 * The main entry point of the application.
 */
int main(int, char**) {
  wen_meminit();

  wen_game_t game_inst{};
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

  wen_memfree(&game_inst.state, sizeof(game_inst.state), MEMORY_TAG_APPLICATION);
  wen_memcleanup();

  return 0;
}

#endif // WEN_ENTRY_HPP_
