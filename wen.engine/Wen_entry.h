#pragma once

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include "Wen/Wen.h"

extern Application* create_application();

FORCE_INLINE SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    auto app = create_application();
    *appstate = app;
    return app->app_init();
}

FORCE_INLINE SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
	if (auto app = reinterpret_cast<Application*>(appstate)) {
    return app->app_event(event);
  }
  return SDL_APP_FAILURE;
}

FORCE_INLINE SDL_AppResult SDL_AppIterate(void *appstate) {
	if (auto app = reinterpret_cast<Application*>(appstate)) {
    return app->app_iterate();
  }
  return SDL_APP_FAILURE;
}

FORCE_INLINE void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    delete reinterpret_cast<Application*>(appstate);
}