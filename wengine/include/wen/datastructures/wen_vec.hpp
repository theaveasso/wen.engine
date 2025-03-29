/**
 * @file vec.hpp
 * @brief Vector data structure.
 */
#ifndef WEN_DATASTRUCTURES_WEN_VEC_HPP_
#define WEN_DATASTRUCTURES_WEN_VEC_HPP_

#include "wen/internal/wen_internal_defines.hpp"

#ifdef __cplusplus
extern "C" {
#endif

enum {
  VEC_CAPACITY,
  VEC_LENGTH,
  VEC_STRIDE,
  VEC_FIELD_LENGTH,
};

/** A component column */
typedef struct wen_vec_t {
  void*   array;
  int32_t count;
  int32_t size;
} wen_vec_t;

WEN_API void* _vec_create(uint64_t len_, uint64_t stride_);
WEN_API void  _vec_destroy(void* arr_);

WEN_API uint64_t _vec_field_get(void* arr_, uint64_t field_);
WEN_API void     _vec_field_set(void* arr_, uint64_t field_, uint64_t val_);

WEN_API void* _vec_push(void* arr_, const void* val_ptr_);
WEN_API void  _vec_pop(void* arr_, uint64_t idx_, void* val_ptr_);

WEN_API void* _vec_pop_at(void* arr_, uint64_t idx_, void* dst_);
WEN_API void* _vec_insert_at(void* arr_, uint64_t idx_, void* val_ptr_);

WEN_API void* _vec_resize(void* arr_);

#define WEN_VEC_DEFAULT_CAPACITY 1
#define WEN_VEC_RESIZE_FACTOR 2

#define vec_create(type) (typeof(type*))_vec_create(WEN_VEC_DEFAULT_CAPACITY, sizeof(type))

#define vec_reserve(type, capacity) _vec_create(capacity, sizeof(type))

#define vec_destroy(array) _vec_destroy(array)

#define vec_push(array, value)                                                 \
  {                                                                            \
    typeof(value) tmp = value;                                                 \
    array             = (typeof(array))_vec_push(array, &tmp);                                \
  }

#define vec_pop(array, value_ptr) _vec_pop(array, value_ptr)

#define vec_pop_at(array, index, value_ptr)                                    \
  array = (typeof(array))_vec_pop_at(array, index, value_ptr);

#define vec_insert_at(array, index, value_ptr)                                 \
  {                                                                            \
    typeof(value) tmp = value;                                                 \
    array             = _vec_insert_at(array, index, &tmp);                    \
  }

#define vec_clear(array) _vec_field_set(array, VEC_LENGTH, 0)

#define vec_capacity(array) _vec_field_get(array, VEC_CAPACITY)

#define vec_len(array) _vec_field_get(array, VEC_LENGTH)

#define vec_stride(array) _vec_field_get(array, VEC_STRIDE)

#define vec_len_set(array, value) _vec_field_set(array, VEC_LENGTH, value)

#ifdef __cplusplus
};
#endif

#endif // WEN_DATASTRUCTURES_WEN_VEC_HPP_
