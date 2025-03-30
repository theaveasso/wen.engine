#ifndef WEN_HPP_
#define WEN_HPP_

/** WEN version macros. */
#define WEN_VERSION_MAJOR 0 /** WEN major version. */
#define WEN_VERSION_MINOR 1 /** WEN minor version. */
#define WEN_VERSION_PATCH 0 /** WEN patch version. */

#if defined(__WIN32)
#define WEN_SYSTEM_WINDOWS
#if !defined(WEN_STATIC)
#define WEN_API __declspec(dllexport) /** building the library as a Win32 shared library (.dll). */
#elif defined(WEN_USE_SHARED)
#define WEN_API __declspec(dllimport) /** using the library as a Win32 shared library (.dll). */
#endif
#else
#if defined(WEN_BUILD_SHARED)
#define WEN_API __attribute__((visibility("default"))) /** building as a Unix shared library (.dll). */
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <cstdint>

/** @def WEN_DEBUG
 * Used for input parameter checking and cheap sanity checks. There are lots of
 * asserts in every part of the code, so this will slow down applications.
 */
#if !defined(WEN_DEBUG) && !defined(WEN_NDEBUG)
#if defined(NDEBUG)
#define WEN_NDEBUG
#else
#define WEN_DEBUG
#endif
#endif

/** @def WEN_SANITIZE
 * Enables expensive check that can detect issues early. Recommended for running
 * tests or when debugging issues. This will severely slow down code.
 */
#ifdef WEN_SANITIZE
#ifndef WEN_DEBUG
#define WEN_DEBUG /** If sanitized mode is enabled, so is debug mode. */
#endif
#endif

/** @def WEN_MESSAGE_CODE_COUNT_MAX
 * Maximum number of message codes. Should not exceed 16384.
 */
#ifndef WEN_MESSAGE_CODE_COUNT_MAX
#define WEN_MESSAGE_CODE_COUNT_MAX 16384
#endif

/** @def WEN_KIB
 * Used for expand 1 Kilobyte value.
 */
#define KIB(VAL) ((VAL) * 1024)

/** @def WEN_MIB
 * Used for expand 1 Megabyte value.
 */
#define MIB(VAL) ((VAL) * KIB(1024))

/** @def WEN_GIB
 * Used for expand 1 Gigabyte value.
 */
#define GIB(VAL) ((VAL) * MIB(1024))

typedef struct {
  int x;
  int y;
} wen_point_t;

typedef struct wen_size_t {
  uint64_t width;
  uint64_t height;
} wen_size_t;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // WEN_HPP_
