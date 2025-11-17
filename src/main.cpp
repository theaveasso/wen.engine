#include "Core.h"
#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "App.h"

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
  auto app = new Application();
  *appstate = app;
  return app->app_init();
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  if (auto app = reinterpret_cast<Application*>(appstate)) {
    return app->app_iterate();
  }
  WEN_ERROR("SDL app iterate failed");
  return SDL_APP_FAILURE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (auto app = reinterpret_cast<Application*>(appstate)) {
    return app->app_event(event);
  }
  WEN_ERROR("SDL app event failed");
  return SDL_APP_FAILURE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  if (auto app = reinterpret_cast<Application*>(appstate)) {
    app->app_quit();
    delete app;
  }
}