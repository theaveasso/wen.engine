/**
 * @file
 * @brief
 *
 * This file
 */
#ifndef PLATFORM_WEN_PLATFORM_HPP_
#define PLATFORM_WEN_PLATFORM_HPP_

#include "wen/wen.hpp"

#if defined(WEN_SYSTEM_WINDOWS)
#include <malloc.h>
#else
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <cstdint>

typedef struct wen_platform_state_t {
  void* internal_state;
} wen_platform_state_t;

/** Platform related functions. (graphics, inputs and more) */
bool   platform_init(wen_platform_state_t* state_, const char* name_, int32_t width_, int32_t height_);
void   platform_shutdown(wen_platform_state_t* state_);
void   platform_sleep(uint32_t milliseconds);
void   platform_poll_event();
bool   platform_is_running();
double platform_get_absolute_time();

/** Memory related functions. */
void* platform_memalloc(uint64_t len_, bool aligned_ = true);
void  platform_memfree(void* block_, bool aligned_);
void* platform_memzero(void* block_, uint64_t len_);
void* platform_memset(void* dst_, int32_t val_, uint64_t dwords_);
void* platform_memcpy(void* dst_, const void* src_, uint64_t len_);
void* platform_memmove(void* dst_, const void* src_, uint64_t len_);

#ifdef __cplusplus
}
#endif
#endif // PLATFORM_WEN_PLATFORM_HPP_
