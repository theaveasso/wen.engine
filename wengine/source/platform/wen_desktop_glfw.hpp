#pragma once

#include <functional>
#include <vector>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

using key_callback    = std::function<void(int key, int scancode, int action, int mods)>;
using button_callback = std::function<void(int button, int action, int mods)>;
using cursor_callback = std::function<void(double x_pos, double y_pos)>;
using resize_callback = std::function<void(int width, int height)>;

struct WenWindow
{
	GLFWwindow *window;

	const char            *title;
	static inline uint32_t width;
	static inline uint32_t height;

	static inline std::vector<key_callback>    key_callbacks;
	static inline std::vector<button_callback> button_callbacks;
	static inline std::vector<cursor_callback> cursor_callbacks;
	static inline std::vector<resize_callback> resize_callbacks;
};

enum struct WenCursorMode
{
	NORMAL   = GLFW_CURSOR_NORMAL,
	HIDDEN   = GLFW_CURSOR_HIDDEN,
	DISABLED = GLFW_CURSOR_DISABLED,
	CAPTURED = GLFW_CURSOR_CAPTURED,
};

void window_init(WenWindow *window, uint32_t width, uint32_t height, const char *title);
void window_fini(WenWindow *window);

void window_register_key_callback(WenWindow *window, key_callback &&on_key_proc);
void window_register_button_callback(WenWindow *window, button_callback &&on_button_proc);
void window_register_cursor_callback(WenWindow *window, cursor_callback &&on_cursor_proc);
void window_register_resize_callback(WenWindow *window, resize_callback &&on_resize_proc);

void window_set_cursor_mode(WenWindow *window, WenCursorMode mode);

VkSurfaceKHR vk_window_get_surface(const WenWindow &window, VkInstance instance);