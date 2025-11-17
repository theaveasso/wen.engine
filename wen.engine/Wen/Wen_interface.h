#pragma once

#include "Wen_core.h"

class WEN_API AppInterface {
  public:
  FORCE_INLINE AppInterface() = default;
  FORCE_INLINE virtual ~AppInterface() = default;

  protected:
  virtual void on_update(float deltatime) = 0;
  virtual void on_draw() = 0;

  virtual void on_init() {};
  virtual void on_deleted() {};
};