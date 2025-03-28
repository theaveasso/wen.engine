#ifndef CORE_WEN_MEMORY_HPP_
#define CORE_WEN_MEMORY_HPP_

#include <cstdint>
#include <string>

#include "wen/wen_defines.hpp"

typedef enum memory_tag_t {
  MEMTAG_UNKNOWN,
  MEMTAG_ARRAY,
  MEMTAG_APPLICATION,
  MEMTAG_BST,
  MEMTAG_DICT,
  MEMTAG_ENTITY,
  MEMTAG_ENTITY_NODE,
  MEMTAG_JOB,
  MEMTAG_MATERIAL_INSTANCE,
  MEMTAG_RING_QUEUE,
  MEMTAG_STRING,
  MEMTAG_SCENE,
  MEMTAG_TEXTURE,
  MEMTAG_VECTOR,
  MEMTAG_RENDERER,

  MEMTAG_MAX_TAGS,
} memory_tag_t;

void WEN_API wen_meminit();

void WEN_API wen_memcleanup();

WEN_API void* wen_memalloc(uint64_t len_, memory_tag_t tag_);

WEN_API void wen_memfree(void* mem_, uint64_t len_, memory_tag_t tag_);

WEN_API void* wen_memzero(void* mem_, uint64_t len_);

WEN_API void* wen_memcpy(void* dst_, const void* src_, uint64_t dwords_);

WEN_API void* wen_memset(void* dst_, int32_t val_, uint64_t len);

std::string get_mem_usage_str();

#endif // CORE_WEN_MEMORY_HPP_
