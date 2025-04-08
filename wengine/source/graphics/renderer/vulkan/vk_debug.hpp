#pragma once

#include "privates/wen_pch.hpp"

#include <vulkan/vulkan.h>

struct WenVkDebugger
{
	VkDebugUtilsMessengerEXT         messenger;
	PFN_vkSetDebugUtilsObjectNameEXT obj_name_pfn;
	VkDevice                         logical_device;
};

VkResult vk_debugger_init(
    WenVkDebugger *debugger,
    VkInstance     instance,
    VkDevice       device);

void vk_debugger_fini(
    const WenVkDebugger *debugger,
    VkInstance           instance);

VkDebugUtilsMessengerCreateInfoEXT vk_messenger_info_init();

VkValidationFeaturesEXT vk_validation_features_init();

std::array<const char *, 1> vk_validation_layers_init();

template <typename T>
void vk_debugger_set_handle_name(
    const WenVkDebugger *debugger,
    T                    handle,
    VkObjectType         object_type,
    std::string_view     name)
{
	VkDebugUtilsObjectNameInfoEXT name_info = {
	    VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT};
	name_info.objectType   = object_type;
	name_info.objectHandle = reinterpret_cast<uint32_t>(handle);
	name_info.pObjectName  = name.data();

	debugger->obj_name_pfn(debugger->logical_device, &name_info);
}
