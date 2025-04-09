#include "vk_shader.hpp"

#include "graphics/renderer/vulkan/common/vk_common.hpp"
#include "vk_pipeline.hpp"

#define BUILTIN_SHADER_NAME_OBJECT "Builtin.ObjectShader"

bool wen_vk_shader_object_init(VkDevice device, VkFormat format, WenVkObjectShader *object_shader)
{
	std::array<std::string, OBJECT_SHADER_STAGE_COUNT>           stage_type_exts = {"vert", "frag"};
	std::array<VkShaderStageFlagBits, OBJECT_SHADER_STAGE_COUNT> stage_types     = {
        VK_SHADER_STAGE_VERTEX_BIT,
        VK_SHADER_STAGE_FRAGMENT_BIT};

	for (uint32_t i = 0; i < OBJECT_SHADER_STAGE_COUNT; ++i)
	{
		object_shader->shader_stages[i].shader_module = wen_load_shader_spirv(
		    device,
		    stage_types[i],
		    BUILTIN_SHADER_NAME_OBJECT);

		object_shader->shader_stages[i].stage.stage  = stage_types[i];
		object_shader->shader_stages[i].stage.module = object_shader->shader_stages[i].shader_module;
		object_shader->shader_stages[i].stage.pName  = "main";
	}

	// initialize graphics pipeline and pipeline layout.
	return vk_pipeline_info_init(
	    device,
	    format,
	    object_shader);
}

void wen_vk_shader_object_fini(
    VkDevice           device,
    WenVkObjectShader *object_shader)
{
	for (auto shader : object_shader->shader_stages)
	{
		vkDestroyShaderModule(
		    device,
		    shader.shader_module,
		    VK_NULL_HANDLE);
	}

	// de-initialize graphics pipeline and pipeline layout.
	vk_pipeline_info_fini(device, &object_shader->pipeline_info);
}