#include "wen_engine.hpp"

#include <cassert>

#include "wen_logger.hpp"

static WenEngine *engine = nullptr;

static void movement_callback();
static void eye_movement_callback();

void engine_init(WenEngine *engine_, WenEngineMode mode) {
    log_init(core_opts::level);
    WEN_ABORT(engine == nullptr, "Engine is already initialized!");

    engine = engine_;
    engine->mode = mode;

    window_init(&engine->window, core_opts::start_width, core_opts::start_height, "Wen Engine");
    renderer_init(core_opts::renderer, &engine->renderer, &engine->window, "Game", core_opts::start_width, core_opts::start_height);

    WEN_INFO("[Engine] successfully initialized!");
}

void engine_fini(WenEngine *engine_) {
    window_fini(&engine_->window);
    renderer_fini(&engine_->renderer);
    WEN_INFO("[Engine] successfully shutdown!");
    log_fini();
}

bool engine_is_running(const WenEngine *engine_) {
    return (!glfwWindowShouldClose(engine_->window.window));
}

void engine_begin_frame() {
    glfwPollEvents();
}

void engine_end_frame(WenEngine *engine_) {
    renderer_draw_frame(&engine_->renderer);
}

void engine_enable_features(WenEngine *engine_, WenEngineFeatures features) {
//    engine_->features |= features;
}

void engine_disable_features(WenEngine *engine_, WenEngineFeatures features) {
//    engine_->features &= ~features;
}
