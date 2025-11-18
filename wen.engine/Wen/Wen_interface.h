#pragma once

#include "Wen_pch.h"

struct WEN_API AppInterface {
  FORCE_INLINE AppInterface() = default;
  FORCE_INLINE virtual ~AppInterface() = default;

  virtual void on_update(float deltatime) = 0;
  virtual void on_draw() = 0;

  virtual void on_init() {};
  virtual void on_deleted() {};

  uint32_t layer_id;
};