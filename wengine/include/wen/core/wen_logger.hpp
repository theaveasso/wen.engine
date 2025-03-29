#ifndef WEN_LOGGER_HPP_
#define WEN_LOGGER_HPP_

#include <SDL3/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Error codes
 */
#define WEN_INVALID_OPERATION (1)
#define WEN_INVALID_PARAMETER (2)
#define WEN_CONSTRAINT_VIOLATED (3)
#define WEN_OUT_OF_MEMORY (4)
#define WEN_OUT_OF_RANGE (5)
#define WEN_UNSUPPORTED (6)
#define WEN_INTERNAL_ERROR (7)
#define WEN_ALREADY_DEFINED (8)
#define WEN_MISSING_PLATFORM_API (9)
#define WEN_OPERATION_FAILED (10)
#define WEN_INVALID_CONVERSION (11)
#define WEN_LEAK_DETECTED (12)
#define WEN_DOUBLE_FREE (13)

#define WEN_INCONSISTENT_NAME (20)

#define WEN_ACCESS_VIOLATION (40)
#define WEN_COLUMN_INDEX_OUT_OF_RANGE (41)

/*
 * Used when logging with colors is enabled
 */
#define WEN_BLACK "\033[1;30m"
#define WEN_RED "\033[0;31m"
#define WEN_GREEN "\033[0;32m"
#define WEN_YELLOW "\033[0;33m"
#define WEN_BLUE "\033[0;34m"
#define WEN_MAGENTA "\033[0;35m"
#define WEN_CYAN "\033[0;36m"
#define WEN_WHITE "\033[1;37m"
#define WEN_GREY "\033[0;37m"
#define WEN_NORMAL "\033[0;49m"
#define WEN_BOLD "\033[1;49m"

/* Assert.
 * Aborts if condition is false, disabled in debug mode.
 */
#if defined(WEN_NDEBUG) && !defined(WEN_KEEP_ASSERT)
#define wen_assert(condition, error_code, ...)
#else
#define wen_assert(condition, error_code, ...)                                 \
  if (!(condition)) {                                                          \
    wen_assert_log_(error_code, #condition, __FILE__, __LINE__, __VAR_ARGS__); \
    wen_os_abort();                                                            \
  }                                                                            \
  assert(condition)
#endif // WEN_NDEBUG && !WEN_KEEP_ASSERT

#define wen_assert_var(var, error_code, ...)                                   \
  wen_assert(var, error_code, __VAR_ARGS__);                                   \
  \ (void)var

/* Debug assert.
 * Assert that is only valid in debug mode (ignores WEN_KEEP_ASSERT).
 */
#if defined(WEN_NDBUG)
#define wen_dbg_assert(condition, error_code, ...)                             \
  wen_assert(condition, error_code, __VA_ARGS__)
#else
#define wen_dbg_assert(condition, error_code, ...)
#endif

#define WEN_FATAL(message, ...)                                                \
  SDL_LogError(SDL_LOG_CATEGORY_ERROR, message, ##__VA_ARGS__)

#define WEN_ERROR(message, ...) SDL_Log("[ERROR]: " message, ##__VA_ARGS__)

#define WEN_WARN(message, ...)                                                 \
  SDL_LogWarn(SDL_LOG_PRIORITY_WARN, message, ##__VA_ARGS__)

#define WEN_INFO(message, ...) SDL_Log(message, ##__VA_ARGS__)

#define WEN_TRACE(message, ...) SDL_Log("[TRACE]: " message, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif // WEN_LOGGER_HPP_
