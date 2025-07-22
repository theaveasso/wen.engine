#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

static SDL_Window	*window		= nullptr;
static SDL_Renderer *renderer	= nullptr;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
	SDL_SetAppMetadata("Wen.Engine", "1.0.0", "com.wen.engine");

	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init: %s\n", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	if (!SDL_CreateWindowAndRenderer("Wen", 800, 600, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY, &window, &renderer))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindowAndRenderer: %s\n", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	if (!SDL_ShowWindow(window))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_ShowWindow: %s\n", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
	if (event->type == SDL_EVENT_QUIT)
	{
		return SDL_APP_SUCCESS;
	}

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
	const auto now = static_cast<double>(SDL_GetTicks()) / 1000.0;
	const auto r = static_cast<float>(0.5 + 0.5 * SDL_sin(now));
	const auto g = static_cast<float>(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 2 / 3));
	const auto b = static_cast<float>(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 4 / 3));

	SDL_SetRenderDrawColorFloat(renderer, r, g, b, SDL_ALPHA_OPAQUE_FLOAT);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
}
