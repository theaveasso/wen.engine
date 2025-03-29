/*
 * @file wen_defines.hpp
 * @brief Supporting defines for the public API
 */

#ifndef WEN_DEFINES_HPP_
#define WEN_DEFINES_HPP_

/** WEN version macros. */
#define WEN_VERSION_MAJOR 0 /** WEN major version. */
#define WEN_VERSION_MINOR 1 /** WEN minor version. */
#define WEN_VERSION_PATCH 0 /** WEN patch version. */

/** WEN version. */
#define WEN_VERSION                                                            \
  WEN_VERSION_IMPL(WEN_VERSION_MAJOR, WEN_VERSION_MINOR, WEN_VERSION_PATCH)

/** @def wen_f32_t
 * Customizable precision for floating point operations.
 */
#ifndef wen_f32_t
#define wen_f32_t float
#endif

/** @def WEN_DEBUG
 * Used for input parameter checking and cheap sanity checks. There are lots of
 * asserts in every part of the code, so this will slow down applications.
 */
#if !defined(WEN_DBUG) && !defined(WEN_NDEBUG)
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

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // WEN_DEFINES_HPP_
