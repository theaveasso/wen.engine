#include <iostream>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include "core/wen_logger.hpp"
#include "graphics/renderer/vulkan/vk_utils.hpp"
#include "wen_desktop_glfw.hpp"

static void error_callback(int error, const char *description);
static void key_callback_(GLFWwindow *window, int key, int scancode, int action, int mods);
static void button_callback_(GLFWwindow *window, int button, int action, int mods);
static void resize_callback_(GLFWwindow *window, int width, int height);
static void cursor_callback_(GLFWwindow *window, double x_pos, double y_pos);

void window_init(WenWindow *window, uint32_t width, uint32_t height, const char *title)
{
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	WenWindow::width  = width;
	WenWindow::height = height;
	window->title     = title;

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window->window = glfwCreateWindow((int) WenWindow::width, (int) WenWindow::height, window->title, nullptr, nullptr);
	if (!window->window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	if (glfwRawMouseMotionSupported())
	{
		glfwSetInputMode(window->window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}

	glfwSetErrorCallback(error_callback);
	glfwSetKeyCallback(window->window, key_callback_);
	glfwSetMouseButtonCallback(window->window, button_callback_);
	glfwSetWindowSizeCallback(window->window, resize_callback_);
	glfwSetCursorPosCallback(window->window, cursor_callback_);

	WEN_DEBUG("[Window] successfully initialized!");
}

void window_fini(WenWindow *window)
{
	glfwDestroyWindow(window->window);
	glfwTerminate();
	WEN_DEBUG("[Window] successfully destroyed!");
}

void window_register_key_callback(WenWindow *window, key_callback &&on_key_proc)
{
	WenWindow::key_callbacks.push_back(on_key_proc);
}

void window_register_button_callback(WenWindow *window, button_callback &&on_button_proc)
{
	WenWindow::button_callbacks.push_back(on_button_proc);
}

void window_register_cursor_callback(WenWindow *window, cursor_callback &&on_cursor_proc)
{
	WenWindow::cursor_callbacks.push_back(on_cursor_proc);
}

void window_register_resize_callback(WenWindow *window, resize_callback &&on_resize_proc)
{
	WenWindow::resize_callbacks.push_back(on_resize_proc);
}

void window_set_cursor_mode(WenWindow *window, WenCursorMode mode)
{
	glfwSetInputMode(window->window, GLFW_CURSOR, static_cast<int>(mode));
}

VkSurfaceKHR vk_window_get_surface(const WenWindow &window, VkInstance instance)
{
	VkSurfaceKHR surface;
	vk_check(glfwCreateWindowSurface(instance, window.window, nullptr, &surface));
	return surface;
}

static void error_callback(int, const char *description)
{
	std::cout << "GLFW error :" << description << "\n";
}

static void key_callback_(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (mods == GLFW_MOD_CONTROL && key == GLFW_KEY_E)
		{
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
		if (key == GLFW_KEY_E)
		{
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
	}
	for (auto &callback : WenWindow::key_callbacks)
	{
		callback(key, scancode, action, mods);
	}
}

static void button_callback_(GLFWwindow *window, int button, int action, int mods)
{
	for (auto &callback : WenWindow::button_callbacks)
	{
		callback(button, action, mods);
	}
}

static void resize_callback_(GLFWwindow *window, int width, int height)
{
	for (auto &callback : WenWindow::resize_callbacks)
	{
		callback(width, height);
	}
	WenWindow::width  = width;
	WenWindow::height = height;
}

static void cursor_callback_(GLFWwindow *window, double x_pos, double y_pos)
{
	for (auto &callback : WenWindow::cursor_callbacks)
	{
		callback(x_pos, y_pos);
	}
}