#pragma once

#include <SDL3/SDL.h>

inline auto SDL_Fail() -> SDL_AppResult {
  SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s", SDL_GetError());
  return SDL_APP_FAILURE;
}