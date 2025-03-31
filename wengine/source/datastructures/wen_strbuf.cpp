#include "wen/datastructures/wen_strbuf.hpp"

#include <cstring>

#include "wen/core/wen_memory.hpp"

WEN_API uint64_t wen_str_len(const char* str) {
  return strlen(str);
}

WEN_API char* wen_str_dup(const char* str) {
  uint64_t len = strlen(str);
  char* cpy = (char*)wen_memalloc(len + 1, MEMORY_TAG_STRING);
  wen_memcpy(cpy, str, len + 1);
  return cpy;
}

WEN_API bool wen_str_eq(const char* str1, const char* str2) {
  return strcmp(str1, str2) == 0;
}
