#pragma once

#include "Wen/Wen_Event.h"
#include "Wen_Core.h"

struct Wen_AppInterface;

struct Wen_AppContext {
    WEN_INLINE ~Wen_AppContext() = default;

    std::vector<std::unique_ptr<Wen_AppInterface>> layers;
    Wen_EventDispatcher dispatcher;
};
