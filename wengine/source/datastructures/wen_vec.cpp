#include "wen/datastructures/wen_vec.hpp"

#include "wen/core/wen_logger.hpp"
#include "wen/core/wen_memory.hpp"

void* _vec_create(uint64_t len_, uint64_t stride_) {
  uint64_t header_size = VEC_FIELD_LENGTH * sizeof(uint64_t);
  uint64_t array_size  = len_ * stride_;

  auto* arr = (uint64_t*)wen_memalloc(array_size, MEMTAG_VECTOR);
  wen_memset(arr, 0, header_size + array_size);
  arr[VEC_CAPACITY] = len_;
  arr[VEC_LENGTH]   = 0;
  arr[VEC_STRIDE]   = stride_;
  return (void*)(arr + VEC_FIELD_LENGTH);
}

void _vec_destroy(void* arr_) {
  auto* header = (uint64_t*)arr_ - VEC_FIELD_LENGTH;

  uint64_t header_size = VEC_FIELD_LENGTH * sizeof(uint64_t);
  uint64_t total_size = header_size + header[VEC_CAPACITY] * header[VEC_STRIDE];
  wen_memfree(arr_, total_size, MEMTAG_VECTOR);
}

uint64_t _vec_field_get(void* arr_, uint64_t field_) {
  auto* header = (uint64_t*)arr_ - VEC_FIELD_LENGTH;
  return header[field_];
}

void _vec_field_set(void* arr_, uint64_t field_, uint64_t val_) {
  auto* header   = (uint64_t*)arr_ - VEC_FIELD_LENGTH;
  header[field_] = val_;
}

void* _vec_push(void* arr_, const void* val_ptr_) {
  uint64_t len    = vec_len(arr_);
  uint64_t stride = vec_stride(arr_);
  if (len >= vec_capacity(arr_)) {
    arr_ = _vec_resize(arr_);
  }

  auto addr = (uint64_t)arr_;
  addr += (len * stride);
  wen_memcpy((void*)addr, val_ptr_, stride);
  _vec_field_set(arr_, VEC_LENGTH, len + 1);
  return arr_;
}

void _vec_pop(void* arr_, uint64_t idx_, void* dst_) {
  uint64_t len    = vec_len(arr_);
  uint64_t stride = vec_stride(arr_);

  auto addr = (uint64_t)arr_;
  addr += ((len - 1) * stride);
  wen_memcpy(dst_, (void*)addr, stride);
  _vec_field_set(arr_, VEC_LENGTH, len - 1);
}

void* _vec_pop_at(void* arr_, uint64_t idx_, void* dst_) {
  uint64_t len    = vec_len(arr_);
  uint64_t stride = vec_stride(arr_);
  if (idx_ >= len) {
    WEN_ERROR("%s", "column index out of range.");
    return arr_;
  }

  auto addr = (uint64_t)arr_;
  wen_memcpy(dst_, (void*)(addr + (idx_ * stride)), stride);

  if (idx_ != len - 1) {
    wen_memcpy((void*)(addr + (idx_ * stride)),
               (void*)(addr + ((idx_ + 1) * stride)),
               stride * (len * (len - idx_)));
  }

  _vec_field_set(arr_, VEC_LENGTH, len - 1);
  return arr_;
}

void* _vec_insert_at(void* arr_, uint64_t idx_, void* val_ptr_) {
  uint64_t len = vec_len(arr_);
  uint64_t stride = vec_stride(arr_);
  if (idx_ >= len) {
    WEN_ERROR("%s", "column index out of range.");
    return arr_;
  }
  if (idx_ >= vec_capacity(arr_)) {
    _vec_resize(arr_);
  }

  auto addr = (uint64_t)arr_;
  if (idx_ != len -1) {
    wen_memcpy((void*)(addr + ((idx_ + 1) * stride)),
               (void*)(addr + (idx_ * stride)),
               stride * (len * (len - idx_)));
  }

  wen_memcpy((void*)(addr + (idx_ * stride)), val_ptr_, stride);

  _vec_field_set(arr_, VEC_LENGTH, len + 1);
  return arr_;
}

void* _vec_resize(void* arr_) {
  uint64_t len    = vec_len(arr_);
  uint64_t stride = vec_stride(arr_);

  void* tmp = _vec_create((WEN_VEC_RESIZE_FACTOR * vec_capacity(arr_)), stride);
  wen_memcpy(tmp, arr_, VEC_LENGTH * stride);

  _vec_field_set(tmp, VEC_LENGTH, len);
  _vec_destroy(arr_);
  return tmp;
}