#ifndef WEN_LOGGER_HPP_
#define WEN_LOGGER_HPP_

#include <SDL3/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  WEN_LOG_CATEGORY_TRACE = SDL_LOG_CATEGORY_CUSTOM,
  WEN_LOG_CATEGORY_DEBUG,
  WEN_LOG_CATEGORY_INFO,
  WEN_LOG_CATEGORY_WARN,
  WEN_LOG_CATEGORY_ERROR,
  WEN_LOG_CATEGORY_FATAL,
} WEN_LOG_CATEGORY;

void log_system_initialize();
void log_system_shutdown();

void wen_trace(const char* fmt, ...);
void wen_debug(const char* fmt, ...);
void wen_info(const char* fmt, ...);
void wen_warn(const char* fmt, ...);
void wen_error(const char* fmt, ...);
void wen_fatal(const char* fmt, ...);

#ifdef __cplusplus
}
#endif

#endif // WEN_LOGGER_HPP_
