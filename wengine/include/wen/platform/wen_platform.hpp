/**
 * @file
 * @brief
 *
 * This file
 */
#ifndef PLATFORM_WEN_PLATFORM_HPP_
#define PLATFORM_WEN_PLATFORM_HPP_

#include "wen/wen_defines.hpp"
#if defined(WEN_SYSTEM_WINDOWS)
#include <malloc.h>
#else
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <cstdint>

//** Generic function pointer type. */
typedef void (*os_proc_t)();

void* pm_memalloc(uint64_t len_, bool aligned_ = true);

void pm_memfree(void* block_, bool aligned_);

void* pm_memzero(void* block_, uint64_t len_);

void* pm_memset(void* dst_, int32_t val_, uint64_t dwords_);

void* pm_memcpy(void* dst_, const void* src_, uint64_t len_);

void* pm_memmove(void* dst_, const void* src_, uint64_t len_);

#ifdef __cplusplus
}
#endif
#endif // PLATFORM_WEN_PLATFORM_HPP_
