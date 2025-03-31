#include "wen/core/wen_logger.hpp"

void log_system_initialize() {
  SDL_SetLogPriorities(SDL_LOG_PRIORITY_TRACE);
}

void log_system_shutdown() {

}

void wen_fatal(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  SDL_LogCritical(WEN_LOG_CATEGORY_FATAL, fmt, args);
  va_end(args);
}

void wen_error(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  SDL_LogError(WEN_LOG_CATEGORY_ERROR, fmt, args);
  va_end(args);
}

void wen_warn(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  SDL_LogWarn(WEN_LOG_CATEGORY_WARN, fmt, args);
  va_end(args);
}

void wen_info(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  SDL_LogInfo(WEN_LOG_CATEGORY_INFO, fmt, args);
  va_end(args);
}

void wen_trace(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  SDL_LogTrace(WEN_LOG_CATEGORY_TRACE, fmt, args);
  va_end(args);
}

void wen_debug(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  SDL_LogDebug(WEN_LOG_CATEGORY_DEBUG, fmt, args);
  va_end(args);
}
