#pragma once

#include "privates/wen_pch.hpp"
#include "vk_context.hpp"

VkPhysicalDevice
vk_physical_device_init(VkInstance instance);

WenDeviceQueueFamilies
vk_queue_families_get(VkPhysicalDevice physical_device, VkSurfaceKHR surface);

VkDevice
vk_logical_device_init(
    VkPhysicalDevice              physical_device,
    const WenDeviceQueueFamilies *queue_families);
void
vk_logical_device_fini(WenVkContext *vk_ctx, VkDevice device);

WenDeviceQueues
vk_device_queues_init(
    VkDevice device, const WenDeviceQueueFamilies *queue_families);
