#pragma once

#include "Wen_Core.h"

struct Wen_AppInterface;

struct Wen_AppContext {
    WEN_INLINE ~Wen_AppContext() = default;

    std::vector<std::unique_ptr<Wen_AppInterface>> layers;
};
