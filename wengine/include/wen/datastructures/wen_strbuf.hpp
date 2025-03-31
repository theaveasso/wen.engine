#ifndef DATASTRUCTURES_WEN_STRBUF_HPP_
#define DATASTRUCTURES_WEN_STRBUF_HPP_

#include "wen/wen.hpp"

WEN_API uint64_t wen_str_len(const char* str);
WEN_API char* wen_str_dup(const char* str);

WEN_API bool wen_str_eq(const char* str1, const char* str2);

#endif // DATASTRUCTURES_WEN_STRBUF_HPP_
