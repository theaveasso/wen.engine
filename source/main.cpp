#define SDL_MAIN_USE_CALLBACKS 1

#include "Application.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

static SDL_Window	*window		= nullptr;
static SDL_Renderer *renderer	= nullptr;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
	const auto app = new Application("Wen.Engine", 800, 600, argc, argv);
	*appstate = app;
	return app->Init();
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
	const auto app = static_cast<Application*>(appstate);
	return app->Event(event);
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
	const auto app = static_cast<Application*>(appstate);
	return app->Iterate();
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
	const auto app = static_cast<Application*>(appstate);
	app->Quit();
}
