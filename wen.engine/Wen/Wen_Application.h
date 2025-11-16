#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <cmath>
#include <string>

#include "Wen_Core.h"
#include "Wen_Context.h"
#include "Wen_Interface.h"

struct WEN_API Wen_Engine: Wen_AppInterface {
    WEN_INLINE Wen_Engine(const std::string& title, int width, int height):
        m_title(std::move(title)), m_width(width), m_height(height),
        m_window(nullptr, &SDL_DestroyWindow),
        m_renderer(nullptr, &SDL_DestroyRenderer) {
        m_layer_id = Wen_TypeId<Wen_Engine>();
        m_context = std::make_unique<Wen_AppContext>();
    }

    WEN_INLINE Wen_Engine(): Wen_Engine("Wen", 800, 600) {}

    WEN_INLINE ~Wen_Engine() = default;

    WEN_INLINE SDL_AppResult app_init() {
        if (!SDL_Init(SDL_INIT_VIDEO)) sdl_failed();

        m_window.reset(SDL_CreateWindow(m_title.c_str(), m_width, m_height, SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_RESIZABLE));
        if (!m_window.get()) sdl_failed();

        m_renderer.reset(SDL_CreateRenderer(m_window.get(), 0));
        if (!m_renderer.get()) sdl_failed();

        return SDL_APP_CONTINUE;
    }

    WEN_INLINE void app_quit() {
        SDL_DestroyRenderer(m_renderer.get());
        SDL_DestroyWindow(m_window.get());

        SDL_Quit();
    }

    WEN_INLINE SDL_AppResult app_iterate() {
        auto time = SDL_GetTicks() / 1000.f;
        auto r = (std::sin(time) + 1) / 2.0 * 255;
        auto g = ((std::sin(time) / 2) + 1) / 2.0 * 255;
        auto b = (std::sin(time) * 2 + 1) / 2.0 * 255;
        SDL_SetRenderDrawColor(m_renderer.get(), r, g, b, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(m_renderer.get());

        SDL_RenderPresent(m_renderer.get());
        return m_is_running;
    }

    WEN_INLINE SDL_AppResult app_event(const SDL_Event *event) {
        if (event->type == SDL_EVENT_QUIT) {
            m_is_running = SDL_APP_SUCCESS;
        } else if (event->type == SDL_EVENT_KEY_DOWN) {
            if (event->key.key == SDLK_ESCAPE) {
                m_is_running = SDL_APP_SUCCESS;
            }
        }

        return SDL_APP_CONTINUE;
    }

    WEN_INLINE Wen_AppContext* get_context() { return m_context.get(); }

    WEN_INLINE void run(float deltaTime) {
        while (true) {
            m_context->dispatcher.poll_events();

            for (auto& layer: m_context->layers) {
                layer->on_update(deltaTime);
            }
        }
    }

    Wen_Logger logger;

    private:
    WEN_INLINE SDL_AppResult sdl_failed() {
        WEN_ERROR(SDL_GetError());
        return SDL_APP_FAILURE;
    }

    private:
    uint32_t m_layer_id;
    std::unique_ptr<Wen_AppContext> m_context;

    std::string m_title;
    int m_width;
    int m_height;

    SDL_AppResult m_is_running = SDL_APP_CONTINUE;

    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> m_window;
    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> m_renderer;
};
