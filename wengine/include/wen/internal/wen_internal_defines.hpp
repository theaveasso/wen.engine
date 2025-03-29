#ifndef WEN_INTERNAL_DEFINES_HPP_
#define WEN_INTERNAL_DEFINES_HPP_

/** Standard library dependencies. */
/** Non-standard but required. If not provided by platform, add manually. */
#ifndef __cplusplus
#include <assert.h>
#include <stdint.h>
#else
#include <cassert>
#include <cstdint>
#endif

#include <string>

#if defined(__APPLE__) && defined(__MACH__)
#define WEN_SYSTEM_MACOS
#elif defined(_WIN32) || defined(_WIN64)
#define WEN_SYSTEM_WINDOWS
#elif defined(__linux__)
#define WEN_SYSTEM_LINUX
#else
#error "Unsupported platform"
#endif

#if !defined(WEN_STATIC)
#if defined(WEN_SYSTEM_MACOS) || defined(WEN_SYSTEM_LINUX)
#define WEN_API __attribute__((__visibility__("default")))
#elif defined(WEN_SYSTEM_WINDOWS)
#define WEN_API __declspec(dllexport)
#else
#error "System not support"
#endif
#else
#define WEN_API
#endif

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * Some symbols are only exported when building in debug build, to enable
 * testing of internal data structures.
 */
#ifndef WEN_NDEBUG
#define WEN_DBG_API
#else
#define WEN_DBG_API
#endif

/**
 * Language support defines.
 */
#if !defined(__cplusplus) && !defined(__bool_true_false_are_defined)
#undef bool
#undef true
#undef false
typedef char bool;
#define false 0
#define true !false
#endif

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // WEN_INTERNAL_DEFINES_HPP_
