#include "wen/platform/wen_platform.hpp"

#include "wen/wen.hpp"

#if defined(WEN_SYSTEM_WINDOWS)
#include <windows.h>

void* platform_memalloc(uint64_t len_, bool aligned_) {
  return malloc(len_);
}

void platform_memfree(void* block_, bool aligned_) {
  if (!block_) {
    return;
  }
  free(block_);
  block_ = nullptr;
}

void* platform_memzero(void* block_, uint64_t len_) {
  return memset(block_, 0, len_);
}

void* platform_memset(void* dst_, int32_t val_, uint64_t dwords_) {
  return memset(dst_, val_, dwords_);
}

void* platform_memcpy(void* dst_, const void* src_, uint64_t len_) {
  return memcpy(dst_, src_, len_);
}

void* platform_memmove(void* dst_, const void* src_, uint64_t len_) {
  return memmove(dst_, src_, len_);
}
#endif