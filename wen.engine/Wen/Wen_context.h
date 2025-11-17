#pragma once

#include "Wen_pch.h"
#include "Wen_interface.h"

struct AppContext {
  FORCE_INLINE ~AppContext() = default; 

  std::vector<std::unique_ptr<AppInterface>> layers;
};