#pragma once

#include "platform/wen_desktop_glfw.hpp"
#include "privates/wen_pch.hpp"
#include "vk_debug.hpp"

struct WenDeviceQueues
{
	VkQueue graphics;
	VkQueue present;
	VkQueue transfer;
	VkQueue compute;
};

struct WenDeviceQueueFamilies
{
	uint32_t graphics;
	uint32_t present;
	uint32_t transfer;
	uint32_t compute;
};

struct WenVkContext
{
	VkInstance             instance{};
	VkAllocationCallbacks *allocator{};
	VkSurfaceKHR           surface{};
	VkPhysicalDevice       physical_device{};
	VkDevice               logical_device{};
	WenDeviceQueues        queues{};
	WenDeviceQueueFamilies queue_families{};
	WenVkDebugger          debugger{};
};

void vk_context_init(WenVkContext *vk_ctx, const WenWindow &window, const char *app_name);

void vk_context_fini(WenVkContext *vk_ctx);
