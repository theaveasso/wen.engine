#include "shader_utils.hpp"
#include "platform/wen_io.hpp"

#include <shaderc/shaderc.h>

static std::string
    parse_shader_file(std::string_view filename);

static std::string
    process_includes(std::string content, std::string_view base_dir);

bool compile_shader_spv(
    shaderc_compiler_t        compiler,
    shaderc_compile_options_t compile_options,
    std::string_view          filename,
    VkShaderStageFlagBits     shader_stage,
    std::vector<uint32_t>    *out_spv)
{
	std::string shader_source = parse_shader_file(filename);

	auto shader_kind          = WEN_CAST(shaderc_shader_kind, shader_stage >> 1);

	auto result               = shaderc_compile_into_spv(
        compiler,
        shader_source.data(),
        shader_source.size(),
        shader_kind,
        filename.data(),
        "main",
        compile_options);
	if (shaderc_result_get_num_errors(result) > 0)
	{
		WEN_ERROR(
		    "shader compile failed: {}",
		    shaderc_result_get_error_message(result));
		return false;
	}

	out_spv->clear();
	out_spv->resize(shaderc_result_get_length(result) / sizeof(uint32_t));
	memcpy(
	    out_spv->data(),
	    shaderc_result_get_bytes(result),
	    shaderc_result_get_length(result));

	shaderc_result_release(result);
	return true;
}

std::string
    parse_shader_file(std::string_view filename)
{
	auto content  = wen_read_file(filename);
	auto base_dir = filename.substr(0, filename.find_last_of('/'));
	content       = process_includes(content, base_dir);
	return content;
}

std::string
    process_includes(std::string content, std::string_view base_dir)
{
	bool found_include = true;
	while (found_include)
	{
		found_include = false;
		size_t pos    = 0;
		while ((pos = content.find("#include", pos)) != std::string::npos)
		{
			size_t start = pos;
			size_t end   = content.find('\n', start);
			if (end == std::string ::npos)
			{
				end = content.length();
			}
			size_t quote_start = content.find('"', start);
			size_t quote_end   = content.find('"', quote_start + 1);
			if (quote_start != std::string::npos &&
			    quote_end != std::string::npos)
			{
				std::string filename = content.substr(
				    quote_start + 1, quote_end - quote_start - 1);
				std::string full_path       = std::string(base_dir) + "/" + filename;
				std::string include_content = wen_read_file(full_path);
				content.replace(start, end - start, include_content);
				found_include = true;
			}
			pos = end;
		}
	}

	return content;
}

VkShaderModule vk_shader_module_init(VkDevice device, std::span<uint32_t> shader_spv)
{
	VkShaderModule shader_module;

	VkShaderModuleCreateInfo shader_module_ci = {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
	shader_module_ci.codeSize                 = shader_spv.size();
	shader_module_ci.pCode                    = shader_spv.data();

	vk_check(vkCreateShaderModule(
	    device,
	    &shader_module_ci,
	    nullptr,
	    &shader_module));

	return shader_module;
}
