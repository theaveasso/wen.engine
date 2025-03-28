#include "wen/platform/wen_platform.hpp"

#include "wen/wen_defines.hpp"

#if defined(WEN_SYSTEM_WINDOWS)
#include <cstdlib>
#include <windows.h>

void* pm_memalloc(uint64_t len_, bool aligned_) {
  return malloc(len_);
}

void pm_memfree(void* block_, bool aligned_) {
  free(block_);
}

void* pm_memzero(void* block_, uint64_t len_) {
  return memset(block_, 0, len_);
}

void* pm_memset(void* dst_, int32_t val_, uint64_t dwords_) {
  return memset(dst_, val_, dwords_);
}

void* pm_memcpy(void* dst_, const void* src_, uint64_t len_) {
  return memcpy(dst_, src_, len_);
}

void* pm_memmove(void* dst_, const void* src_, uint64_t len_) {
  return memmove(dst_, src_, len_);
}

#endif