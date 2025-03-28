#ifndef WEN_LOGGER_HPP_
#define WEN_LOGGER_HPP_

#include <SDL3/SDL.h>

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_RESET "\x1b[0m"

typedef enum {
  LOG_LEVEL_FATAL = 0,
  LOG_LEVEL_WARN  = 1,
  LOG_LEVEL_INFO  = 2,
  LOG_LEVEL_DEBUG = 3,
  LOG_LEVEL_TRACE = 4,
} log_level;

#define WEN_FATAL(message, ...)                                                \
  SDL_Log(ANSI_COLOR_RED "\n[FATAL]: " message ANSI_COLOR_RESET, ##__VA_ARGS__)

#define WEN_ERROR(message, ...)                                                \
  SDL_Log(ANSI_COLOR_RED "\n[ERROR]: " message ANSI_COLOR_RESET, ##__VA_ARGS__)

#define WEN_WARN(message, ...)                                                 \
  SDL_Log(ANSI_COLOR_YELLOW "[WARN]: " message ANSI_COLOR_RESET,             \
          ##__VA_ARGS__)

#define WEN_INFO(message, ...)                                                 \
  SDL_Log(ANSI_COLOR_GREEN "\n[INFO]: " message ANSI_COLOR_RESET, ##__VA_ARGS__)

#define WEN_TRACE(message, ...) \
  SDL_Log(ANSI_COLOR_BLUE "[TRACE]: " message ANSI_COLOR_RESET, ##__VA_ARGS__)

#endif // WEN_LOGGER_HPP_
