#include "wen/datastructures/wen_vec.hpp"

#include "wen/core/wen_logger.hpp"
#include "wen/core/wen_memory.hpp"

void* m_vec_create(uint64_t len_, uint64_t stride_) {
  uint64_t header_size = VEC_FIELD_LENGTH * sizeof(uint64_t);
  uint64_t array_size  = len_ * stride_;

  auto* arr = (uint64_t*)wen_memalloc(header_size + array_size, MEMORY_TAG_VECTOR);
  wen_memset(arr, 0, header_size + array_size);
  arr[VEC_CAPACITY] = len_;
  arr[VEC_LENGTH]   = 0;
  arr[VEC_STRIDE]   = stride_;
  return (void*)(arr + VEC_FIELD_LENGTH);
}

void m_vec_destroy(void* arr_) {
  if (!arr_) {
    return;
  }

  auto* header = (uint64_t*)arr_ - VEC_FIELD_LENGTH;

  uint64_t header_size = VEC_FIELD_LENGTH * sizeof(uint64_t);
  uint64_t total_size = header_size + header[VEC_CAPACITY] * header[VEC_STRIDE];
  wen_memfree(header, total_size, MEMORY_TAG_VECTOR);
}

uint64_t m_vec_field_get(void* arr_, uint64_t field_) {
  auto* header = (uint64_t*)arr_ - VEC_FIELD_LENGTH;
  return header[field_];
}

void m_vec_field_set(void* arr_, uint64_t field_, uint64_t val_) {
  auto* header   = (uint64_t*)arr_ - VEC_FIELD_LENGTH;
  header[field_] = val_;
}

void* m_vec_push(void* arr_, const void* val_ptr_) {
  uint64_t len    = vec_len(arr_);
  uint64_t stride = vec_stride(arr_);
  if (len >= vec_capacity(arr_)) {
    arr_ = m_vec_resize(arr_);
  }

  auto addr = (uint64_t)arr_;
  addr += (len * stride);
  wen_memcpy((void*)addr, val_ptr_, stride);
  m_vec_field_set(arr_, VEC_LENGTH, len + 1);
  return arr_;
}

void m_vec_pop(void* arr_, uint64_t idx_, void* dst_) {
  uint64_t len    = vec_len(arr_);
  uint64_t stride = vec_stride(arr_);

  auto addr = (uint64_t)arr_;
  addr += ((len - 1) * stride);
  wen_memcpy(dst_, (void*)addr, stride);
  m_vec_field_set(arr_, VEC_LENGTH, len - 1);
}

void* m_vec_pop_at(void* arr_, uint64_t idx_, void* dst_) {
  uint64_t len    = vec_len(arr_);
  uint64_t stride = vec_stride(arr_);
  if (idx_ >= len) {
    wen_error("column index out of range.");
    return arr_;
  }

  auto addr = (uint64_t)arr_;
  wen_memcpy(dst_, (void*)(addr + (idx_ * stride)), stride);

  if (idx_ != len - 1) {
    wen_memcpy((void*)(addr + (idx_ * stride)),
               (void*)(addr + ((idx_ + 1) * stride)),
               stride * (len * (len - idx_)));
  }

  m_vec_field_set(arr_, VEC_LENGTH, len - 1);
  return arr_;
}

void* m_vec_insert_at(void* arr_, uint64_t idx_, void* val_ptr_) {
  uint64_t len = vec_len(arr_);
  uint64_t stride = vec_stride(arr_);
  if (idx_ >= len) {
    wen_error("column index out of range.");
    return arr_;
  }
  if (idx_ >= vec_capacity(arr_)) {
    m_vec_resize(arr_);
  }

  auto addr = (uint64_t)arr_;
  if (idx_ != len -1) {
    wen_memcpy((void*)(addr + ((idx_ + 1) * stride)),
               (void*)(addr + (idx_ * stride)),
               stride * (len * (len - idx_)));
  }

  wen_memcpy((void*)(addr + (idx_ * stride)), val_ptr_, stride);

  m_vec_field_set(arr_, VEC_LENGTH, len + 1);
  return arr_;
}

void* m_vec_resize(void* arr_) {
  uint64_t len    = vec_len(arr_);
  uint64_t stride = vec_stride(arr_);

  void* tmp = m_vec_create((WEN_VEC_RESIZE_FACTOR * vec_capacity(arr_)), stride);
  wen_memcpy(tmp, arr_, VEC_LENGTH * stride);

  m_vec_field_set(tmp, VEC_LENGTH, len);
  m_vec_destroy(arr_);
  return tmp;
}