#include "vk_common.hpp"

#include "../vk_utils.hpp"

#include "platform/wen_filesystem.hpp"

uint32_t wen_find_memory_type(
    VkPhysicalDevice gpu,
    uint32_t         type_filter,
    int32_t          properties)
{
	// Structure to hold the physical device's memory properties
	VkPhysicalDeviceMemoryProperties memory_properties;
	vkGetPhysicalDeviceMemoryProperties(gpu, &memory_properties);

	// Iterate over all memory types available on the gpu
	for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i)
	{
		if (type_filter & (1 << i))
		{
			if ((memory_properties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}
	}

	throw std::runtime_error("failed to find suitable memory type.");
}

inline std::string get_stage_ext(VkShaderStageFlagBits stage)
{
	switch (stage)
	{
		case VK_SHADER_STAGE_VERTEX_BIT:
			return ".vert";
		case VK_SHADER_STAGE_FRAGMENT_BIT:
			return ".frag";
		default:
			return ".vert";
	}
}

VkShaderModule wen_load_shader_spirv(
    VkDevice              device,
    VkShaderStageFlagBits stage,
    const std::string    &filename)
{
	auto path                            = filename + get_stage_ext(stage);
	auto spirv                           = wen_read_shader_spirv(path);

	VkShaderModuleCreateInfo create_info = {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
	create_info.codeSize                 = spirv.size() * sizeof(uint32_t);
	create_info.pCode                    = spirv.data();

	VkShaderModule shader_module;
	vk_check(vkCreateShaderModule(
	    device,
	    &create_info,
	    nullptr,
	    &shader_module));

	return shader_module;
}
