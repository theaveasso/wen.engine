#ifndef CORE_WEN_MEMORY_HPP_
#define CORE_WEN_MEMORY_HPP_

#include <cstdint>
#include <string>

#include "wen/wen.hpp"

typedef enum memory_tag_t {
  MEMORY_TAG_UNKNOWN,
  MEMORY_TAG_ARRAY,
  MEMORY_TAG_APPLICATION,
  MEMORY_TAG_BST,
  MEMORY_TAG_DICT,
  MEMORY_TAG_ENTITY,
  MEMORY_TAG_ENTITY_NODE,
  MEMORY_TAG_JOB,
  MEMORY_TAG_MATERIAL_INSTANCE,
  MEMORY_TAG_RING_QUEUE,
  MEMORY_TAG_STRING,
  MEMORY_TAG_SCENE,
  MEMORY_TAG_TEXTURE,
  MEMORY_TAG_VECTOR,
  MEMORY_TAG_RENDERER,

  MEMORY_TAG_COUNT,
} memory_tag_t;

void WEN_API  wen_meminit();
void WEN_API  wen_memcleanup();
WEN_API void* wen_memalloc(uint64_t len_, memory_tag_t tag_);
WEN_API void  wen_memfree(void* mem_, uint64_t len_, memory_tag_t tag_);
WEN_API void* wen_memzero(void* mem_, uint64_t len_);
WEN_API void* wen_memcpy(void* dst_, const void* src_, uint64_t dwords_);
WEN_API void* wen_memset(void* dst_, int32_t val_, uint64_t len);

std::string get_mem_usage_str();

#endif // CORE_WEN_MEMORY_HPP_
