#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <Wen/Wen.h>

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    auto app = new Wen_Engine();
    *appstate = app;
    return app->app_init();
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
	const auto app = reinterpret_cast<Wen_Engine*>(appstate);
	return app->app_event(event);
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    const auto app = reinterpret_cast<Wen_Engine*>(appstate);
    return app->app_iterate();
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    auto app = reinterpret_cast<Wen_Engine*>(appstate);
    app->app_quit();
    delete app;
}
