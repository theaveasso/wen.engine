/**
 * @file vec.hpp
 * @brief Vector data structure.
 */
#ifndef WEN_DATASTRUCTURES_WEN_VEC_HPP_
#define WEN_DATASTRUCTURES_WEN_VEC_HPP_

#include "wen/wen.hpp"

enum {
  VEC_CAPACITY,
  VEC_LENGTH,
  VEC_STRIDE,
  VEC_FIELD_LENGTH,
};

WEN_API void* m_vec_create(uint64_t len_, uint64_t stride_);
WEN_API void  m_vec_destroy(void* arr_);

WEN_API uint64_t m_vec_field_get(void* arr_, uint64_t field_);
WEN_API void     m_vec_field_set(void* arr_, uint64_t field_, uint64_t val_);

WEN_API void* m_vec_push(void* arr_, const void* val_ptr_);
WEN_API void  m_vec_pop(void* arr_, uint64_t idx_, void* val_ptr_);

WEN_API void* m_vec_pop_at(void* arr_, uint64_t idx_, void* dst_);
WEN_API void* m_vec_insert_at(void* arr_, uint64_t idx_, void* val_ptr_);

WEN_API void* m_vec_resize(void* arr_);

#define WEN_VEC_DEFAULT_CAPACITY 1
#define WEN_VEC_RESIZE_FACTOR 2

#define vec_create(type) (typeof(type*))m_vec_create(WEN_VEC_DEFAULT_CAPACITY, sizeof(type))

#define vec_reserve(type, capacity) m_vec_create(capacity, sizeof(type))

#define vec_destroy(array) m_vec_destroy(array)

#define vec_push(array, value)                                  \
  {                                                             \
    typeof(value) tmp = value;                                  \
    array             = (typeof(array))m_vec_push(array, &tmp); \
  }

#define vec_pop(array, value_ptr) m_vec_pop(array, value_ptr)

#define vec_pop_at(array, index, value_ptr) \
  array = (typeof(array))m_vec_pop_at(array, index, value_ptr);

#define vec_insert_at(array, index, value_ptr)               \
  {                                                          \
    typeof(value) tmp = value;                               \
    array             = m_vec_insert_at(array, index, &tmp); \
  }

#define vec_clear(array) m_vec_field_set(array, VEC_LENGTH, 0)

#define vec_capacity(array) m_vec_field_get(array, VEC_CAPACITY)

#define vec_len(array) m_vec_field_get(array, VEC_LENGTH)

#define vec_stride(array) m_vec_field_get(array, VEC_STRIDE)

#define vec_len_set(array, value) m_vec_field_set(array, VEC_LENGTH, value)

#endif // WEN_DATASTRUCTURES_WEN_VEC_HPP_
