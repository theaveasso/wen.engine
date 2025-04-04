#pragma once

#include "core/wen_engine.hpp"

int main() {
    WenEngine engine{};

    engine_init(&engine, WenEngineMode::RELEASE);
    while(engine_is_running(&engine)) {
        engine_begin_frame();
        engine_end_frame(&engine);
    }

    engine_fini(&engine);
}