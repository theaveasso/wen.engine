#include "wvk_application.hpp"
#include "common/wvk_defines.hpp"

#include <GLFW/glfw3.h>
#include <imgui.h>

namespace wvk
{
void Application::init(const AppConfig& config)
{
	WVK_PROFILER_ZONE("Application Initialization", WVK_PROFILER_COLOR_CREATE);
	_app_config = config;

	load_app_settings();

	LoggerInit(wvk::LogLevel::DEBUG);

	if (_app_config.windowSize == glm::ivec2{})
	{
		WVK_ASSERT_MSG(_app_config.windowSize != glm::ivec2{}, "window size can not be 0");
		_app_config.windowSize = _app_config.renderArea;
	}
	if (_app_config.renderArea == glm::ivec2{})
	{
		WVK_ASSERT_MSG(_app_config.renderArea != glm::ivec2{}, "render area can not be 0");
		_app_config.renderArea = _app_config.renderArea;
	}
	if (_app_config.windowTitle.empty())
	{
		_app_config.windowTitle = _app_config.appName;
	}

	// init glfw window
	if (!glfwInit())
	{
		WVK_ERROR("failed to initialize GLFW!");
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	_window = glfwCreateWindow(
	    _app_config.windowSize.x,
	    _app_config.windowSize.y,
	    _app_config.windowTitle.c_str(),
	    nullptr,
	    nullptr);
	if (!_window)
	{
		WVK_ERROR("failed to create glfw window!");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	_instance.init(_window, _app_config.appName.c_str(), _vsync);

	on_init();

	glfwSetKeyCallback(_window, [](GLFWwindow *window, int key, int, int action, int) {
		// core engine behavior
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, true);
		}

		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
		}

		if (key == GLFW_KEY_A && action == GLFW_PRESS)
		{
		}

		if (key == GLFW_KEY_S && action == GLFW_PRESS)
		{
		}

		if (key == GLFW_KEY_D && action == GLFW_PRESS)
		{
		}
	});

	WVK_PROFILER_ZONE_END();
}
void Application::cleanup()
{
	on_cleanup();

	_instance.cleanup();
	glfwDestroyWindow(_window);
	glfwTerminate();
}
void Application::run()
{
	const float FPS   = 60.f;
	const float dt    = 1.0f / FPS;

	auto  prevTime    = std::chrono::high_resolution_clock ::now();
	float accumulator = dt;

	_is_running       = true;
	while (!glfwWindowShouldClose(_window))
	{
		const auto newTime = std::chrono::high_resolution_clock ::now();
		_frame_time        = std::chrono::duration<float>(newTime - prevTime).count();

		accumulator += _frame_time;
		prevTime     = newTime;

		float newFPS = 1.0f / _frame_time;
		if (newFPS == std::numeric_limits<float>::infinity())
		{
			newFPS = 0;
		}
		if (accumulator >= 10 * dt)
		{
			accumulator = dt;
		}

		{        // event processing
			while (accumulator >= dt)
			{
				glfwPollEvents();
				if (_instance.required_swapchain_reinit())
				{
					_instance.recreate_swapchain(1280, 232);
					on_window_resize();
				}

				accumulator -= dt;
			}

			_instance.imgui_begin_frame();

			on_update(dt);

			_instance.imgui_end_frame();
		}

		if (!_instance.required_swapchain_reinit())
		{
			on_draw();
		}

		if (_frame_limit)
		{
			const auto now       = std::chrono::high_resolution_clock ::now();
			const auto frameTime = std::chrono::duration<float>(now - prevTime).count();
			if (dt > frameTime)
			{
				// delay
			}
		}
	}
}
void Application::load_dev_settings(const std::filesystem::path &configPath)
{
}

}        // namespace wvk
