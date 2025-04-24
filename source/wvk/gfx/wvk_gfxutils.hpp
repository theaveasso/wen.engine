#pragma once

#include <glm/vec4.hpp>
#include <imgui.h>
#include <volk.h>

#include "wvk/common/wvk_pch.hpp"
#include "wvk_gfxtypes.hpp"

namespace wvk::gfx
{

class Texture;

/**
 * @brief
 */
#pragma region util::Vulkan

/**
 * @brief
 */
const char *wvk_vk_err_string(VkResult result);

#define vk_assert(func)                                                         \
	do                                                                          \
	{                                                                           \
		const VkResult vk_assert_result = func;                                 \
		if (vk_assert_result != VK_SUCCESS)                                     \
		{                                                                       \
			WVK_WARN("Vulkan API call failed: {} {} {} {}", __FILE__, __LINE__, \
			         #func, wvk_vk_err_string(vk_assert_result));               \
			assert(false);                                                      \
		}                                                                       \
	} while (0)

#if defined(WVK_DBG)
void WVK_API cmd_begin_w_label(VkCommandBuffer cmd, const char *label, float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);
void WVK_API cmd_end_w_label(VkCommandBuffer cmd);

template <typename T>
void add_debug_w_label(VkDevice device, T handle, VkObjectType type, std::string_view name)
{
	VkDebugUtilsObjectNameInfoEXT objectNameInfo = {};
	objectNameInfo.sType                         = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	objectNameInfo.objectType                    = type;
	objectNameInfo.objectHandle                  = WVK_RCAST(uint64_t, handle);
	objectNameInfo.pObjectName                   = name.data();
	vk_assert(vkSetDebugUtilsObjectNameEXT(device, &objectNameInfo));
}
#else
#	define cmd_begin_w_label(...) (void) 0
#	define cmd_end_w_label(...) (void) 0
#	define add_debug_w_label(...) (void) 0
#endif

#pragma endregion

/**
 * @brief
 */
#pragma region util::Image

uint32_t bytes_per_pixel(VkFormat format);

void WVK_API transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);

void WVK_API blit_image(VkCommandBuffer cmd, VkImage src, VkImage dst, VkExtent2D src_extents, VkExtent2D dst_extents, VkFilter filter);

void WVK_API generate_mipmaps();

#pragma endregion

/**
 * @brief
 */
#pragma region util::Rendering

uint32_t to_bindless_texture_id(ImTextureID id);

RenderInfo WVK_API create_rendering_info(const RenderingInfo &renderingInfo);

void WVK_API clear(VkCommandBuffer cmd, const std::shared_ptr<Texture> &drawImage, const glm::vec4 &clearValue);

#pragma endregion

/**
 * @brief
 */
#pragma region util::Shader

VkShaderModule load_shader_module(VkDevice device, std::string_view filepath, std::string_view name = "");

#pragma endregion

/**
 * @brief
 */
#pragma region util::Pipeline

VkPipelineLayout create_pipeline_layout(VkDevice device, std::span<const VkDescriptorSetLayout> layouts, std::span<const VkPushConstantRange> pushConstantRanges, std::string_view name = "");

class PipelineBuilder
{
  public:
	explicit PipelineBuilder(VkPipelineLayout pipelineLayout);

	VkPipeline build(VkDevice device);

	PipelineBuilder &set_name(std::string_view name = "");

	PipelineBuilder &set_shaders(VkShaderModule vertexShader, VkShaderModule fragmentShader);

	PipelineBuilder &
	    set_shaders(VkShaderModule vertexShader, VkShaderModule geometryShader, VkShaderModule fragmentShader);

	PipelineBuilder &set_vertex_input(std::span<const VkVertexInputBindingDescription> &bindings, std::span<const VkVertexInputAttributeDescription> &attributes);

	PipelineBuilder &set_input_topology(VkPrimitiveTopology topology);

	PipelineBuilder &set_polygon_mode(VkPolygonMode mode);

	PipelineBuilder &set_cull_mode(VkCullModeFlags cullMode, VkFrontFace frontFace);

	PipelineBuilder &enable_culling();

	PipelineBuilder &disable_culling();

	PipelineBuilder &set_multisampling_none();

	PipelineBuilder &set_multisampling(VkSampleCountFlagBits samples);

	PipelineBuilder &enable_blending(VkBlendOp blendOp = VK_BLEND_OP_ADD, VkBlendFactor srcBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA, VkBlendFactor dstBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VkBlendFactor srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE, VkBlendFactor dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA);

	PipelineBuilder &disable_blending();

	PipelineBuilder &set_color_attachment_format(VkFormat format);

	PipelineBuilder &set_depth_format(VkFormat format);

	PipelineBuilder &enable_depth(bool depthWriteEnable, VkCompareOp op);

	PipelineBuilder &disable_depth_test();

	PipelineBuilder &enable_depth_clamp();

	PipelineBuilder &enable_dynamic_depth();

	PipelineBuilder &enable_depth_bias(float constantFactor, float slopeFactor);

  private:
	void clear();

	std::string _debug_name;

	std::vector<VkPipelineShaderStageCreateInfo>       _shader_stages;
	std::span<const VkVertexInputBindingDescription>   _vertex_bindings;
	std::span<const VkVertexInputAttributeDescription> _vertex_attributes;

	VkPipelineInputAssemblyStateCreateInfo _input_assembly          = {};
	VkPipelineRasterizationStateCreateInfo _rasterization           = {};
	VkPipelineColorBlendAttachmentState    _color_attachment_blend  = {};
	VkPipelineMultisampleStateCreateInfo   _multisample             = {};
	VkPipelineDepthStencilStateCreateInfo  _depth_stencil           = {};
	VkPipelineRenderingCreateInfo          _render_info             = {};
	VkFormat                               _color_attachment_format = VK_FORMAT_UNDEFINED;
	VkPipelineLayout                       _pipeline_layout         = VK_NULL_HANDLE;
	bool                                   _dynamic_depth           = false;
};

#pragma endregion

}        // namespace wvk::gfx
