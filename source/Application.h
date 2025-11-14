#pragma once

#include <SDL3/SDL.h>
#include <string>
#include <memory>

class Application {
public:
	Application(std::string title, int width, int height, int argc, char** argv);

	~Application() = default;

	auto Init() -> SDL_AppResult;
	auto Event(const SDL_Event *event) const -> SDL_AppResult;
	[[nodiscard]] auto Iterate() const -> SDL_AppResult;
	void Quit() const;

private:
	std::string title;

	int width, height;
 
  SDL_WindowFlags window_flags = 
    SDL_WINDOW_HIDDEN |
    SDL_WINDOW_HIGH_PIXEL_DENSITY |
    SDL_WINDOW_RESIZABLE |
    SDL_WINDOW_ALWAYS_ON_TOP;

	std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window;
	std::unique_ptr<SDL_GPUDevice, decltype(&SDL_DestroyGPUDevice)> gpuDevice;

	SDL_GPUPresentMode presentMode = SDL_GPU_PRESENTMODE_VSYNC;

	[[nodiscard]] auto OnQuit() const -> SDL_AppResult;

	[[nodiscard]] auto OnDraw() const -> SDL_AppResult;

	[[nodiscard]] auto OnUpdate() const -> SDL_AppResult;
};
