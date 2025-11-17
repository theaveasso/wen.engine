#pragma once

#include "Core.h"
#include <SDL3/SDL_video.h>
#include <memory>
#include <string>
#include <glad/glad.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

class Application {
  public:
  FORCE_INLINE Application(): Application("Wen.Engine", 800, 600) {}

  FORCE_INLINE Application(const std::string& title, int width, int height): 
   m_title(std::move(title)), m_width(width), m_height(height),
   m_window(nullptr, &SDL_DestroyWindow)
  {
    m_logger.init();
  }

  FORCE_INLINE ~Application() = default;

  FORCE_INLINE SDL_AppResult app_init() {
    if (!SDL_Init(SDL_INIT_VIDEO)) sdl_fail();

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    m_window.reset(SDL_CreateWindow(m_title.c_str(), m_width, m_height, m_window_flags));
    if (!m_window.get()) sdl_fail();

    SDL_GLContext m_glcontext = SDL_GL_CreateContext(m_window.get());
    if (!m_glcontext) sdl_fail();
    SDL_GL_MakeCurrent(m_window.get(), m_glcontext);

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) sdl_fail();
    WEN_INFO("OpenGL Version: {}.{}", GLVersion.major, GLVersion.minor);
    WEN_INFO("OpenGL Shading Language Version: {}", (char *)glGetString(GL_SHADING_LANGUAGE_VERSION));
    WEN_INFO("OpenGL Vendor: {}", (char *)glGetString(GL_VENDOR));
    WEN_INFO("OpenGL Renderer: {}", (char *)glGetString(GL_RENDERER));

    if (!SDL_ShowWindow(m_window.get())) sdl_fail();

    return SDL_APP_CONTINUE;
  }

  FORCE_INLINE SDL_AppResult app_iterate() {
    glClearColor(0.1f, 0.15f, 0.3f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    SDL_GL_SwapWindow(m_window.get());
    return m_isrunning;
  }

  FORCE_INLINE SDL_AppResult app_event(const SDL_Event* event) {
    switch (event->type) {
      case (SDL_EVENT_QUIT): 
       m_isrunning = SDL_APP_SUCCESS;
       break;
      case (SDL_EVENT_KEY_DOWN):
       switch (event->key.key) {
        case (SDLK_ESCAPE):
         m_isrunning = SDL_APP_SUCCESS;
         break;
       }
      default: 
       break;
    }

    return SDL_APP_CONTINUE;
  }

  FORCE_INLINE void app_quit() {
    SDL_Quit(); 
  }

  private: 
  FORCE_INLINE SDL_AppResult sdl_fail() {
    WEN_ERROR(SDL_GetError());
    return SDL_APP_FAILURE;
  }

  private:
  std::string m_title;
  int m_width, m_height;

  SDL_AppResult m_isrunning = SDL_APP_CONTINUE;

  SDL_WindowFlags m_window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_HIDDEN;

  Logger m_logger;

  std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> m_window;
};