#include "Application.h"
#include "Wen_Utils.h"

#include <array>

Application::Application(std::string title, const int width, const int height, int argc, char** argv) :
    title(std::move(title)), width(width), height(height), window(nullptr, &SDL_DestroyWindow), gpuDevice(nullptr, &SDL_DestroyGPUDevice)
{
}

auto Application::Init() -> SDL_AppResult
{
	SDL_SetAppMetadata(title.c_str(), "1.0.0", "com.theaveasso.wen");

	if (!SDL_Init(SDL_INIT_VIDEO))
	{
    SDL_Fail();
	}

	window.reset(SDL_CreateWindow(
    title.c_str(),
    width,
    height,
    window_flags));
	if (!window)
	{
    SDL_Fail();
	}

#ifdef DEBUG
	SDL_Log("Available GPU Driver:");
	for (int i = 0; i < SDL_GetNumGPUDrivers(); ++i)
	{
		SDL_Log("    - %s", SDL_GetGPUDriver(i));
	}
#endif

	gpuDevice.reset(SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL, true, nullptr));
	if (!gpuDevice)
	{
    SDL_Fail();
	}

	if (!SDL_ClaimWindowForGPUDevice(gpuDevice.get(), window.get()))
	{
    SDL_Fail();
	}

	if (SDL_WindowSupportsGPUPresentMode(gpuDevice.get(), window.get(), SDL_GPU_PRESENTMODE_MAILBOX))
	{
		presentMode = SDL_GPU_PRESENTMODE_MAILBOX;
	}

	if (!SDL_SetGPUSwapchainParameters(gpuDevice.get(), window.get(), SDL_GPU_SWAPCHAINCOMPOSITION_SDR, presentMode))
	{
    SDL_Fail();
	}

	if (!SDL_ShowWindow(window.get()))
	{
    SDL_Fail();
	}

	return SDL_APP_CONTINUE;
}

auto Application::Event(const SDL_Event *event) const -> SDL_AppResult
{
	switch (event->type)
	{
		case SDL_EVENT_QUIT:
			return OnQuit();

		case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
			if (SDL_GetWindowID(window.get()) == event->window.windowID)
			{
				return OnQuit();
			}
			break;

		case SDL_EVENT_KEY_DOWN:
			switch (event->key.key)
			{
				case SDLK_ESCAPE: return OnQuit();
				default:
					break;
			}

		default:
			break;
	}

	return SDL_APP_CONTINUE;
}

auto Application::Iterate() const -> SDL_AppResult
{
	// if (const auto result = OnUpdate(); result != SDL_APP_CONTINUE)
	// {
	// 	return result;
	// }

	if (const auto result = OnDraw(); result != SDL_APP_CONTINUE)
	{
		return result;
	}

	return SDL_APP_CONTINUE;
}

auto Application::Quit() const -> void
{
	SDL_ReleaseWindowFromGPUDevice(gpuDevice.get(), window.get());
}

auto Application::OnUpdate() const -> SDL_AppResult
{
	return SDL_APP_CONTINUE;
}


auto Application::OnDraw() const -> SDL_AppResult
{
	const auto cmd = SDL_AcquireGPUCommandBuffer(gpuDevice.get());

	SDL_GPUTexture *swapchainTexture;
	if (!SDL_AcquireGPUSwapchainTexture(cmd, window.get(), &swapchainTexture, nullptr, nullptr))
	{
    SDL_Fail();
	}

	if (swapchainTexture)
	{
		const auto now = static_cast<double>(SDL_GetTicks()) / 1000.0;
		const auto r   = static_cast<float>(0.5 + 0.5 * SDL_sin(now));
		const auto g   = static_cast<float>(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 2 / 3));
		const auto b   = static_cast<float>(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 4 / 3));

		const std::array<SDL_GPUColorTargetInfo, 1> colorTargetInfos{
		    SDL_GPUColorTargetInfo{
		        .texture     = swapchainTexture,
		        .clear_color = SDL_FColor{r, g, b, 1},
		        .load_op     = SDL_GPU_LOADOP_CLEAR,
		        .store_op    = SDL_GPU_STOREOP_STORE}};

		const auto renderPass = SDL_BeginGPURenderPass(cmd, colorTargetInfos.data(), colorTargetInfos.size(), nullptr);
		SDL_EndGPURenderPass(renderPass);

		if (!SDL_SubmitGPUCommandBuffer(cmd))
		{
      SDL_Fail();
		}
	}

	return SDL_APP_CONTINUE;
}

SDL_AppResult Application::OnQuit() const
{
	SDL_WaitForGPUIdle(gpuDevice.get());
	return SDL_APP_SUCCESS;
}
