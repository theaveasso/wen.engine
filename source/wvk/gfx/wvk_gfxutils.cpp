#include "wvk_gfxutils.hpp"

#include <glm/vec4.hpp>

#include "wvk/common/wvk_filesystem.hpp"
#include "wvk_gfxvkinit.hpp"

#include "wvk/gfx/wvk_gfxvulkan.hpp"

namespace wvk::gfx
{

const char *wvk_vk_err_string(VkResult result)
{
	switch (result)
	{
		case VK_SUCCESS:
			return "VK_SUCCESS";
		case VK_NOT_READY:
			return "VK_NOT_READY";
		case VK_TIMEOUT:
			return "VK_TIMEOUT";
		case VK_EVENT_SET:
			return "VK_EVENT_SET";
		case VK_EVENT_RESET:
			return "VK_EVENT_RESET";
		case VK_INCOMPLETE:
			return "VK_INCOMPLETE";
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			return "VK_ERROR_OUT_OF_HOST_MEMORY";
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
		case VK_ERROR_INITIALIZATION_FAILED:
			return "VK_ERROR_INITIALIZATION_FAILED";
		case VK_ERROR_DEVICE_LOST:
			return "VK_ERROR_DEVICE_LOST";
		case VK_ERROR_MEMORY_MAP_FAILED:
			return "VK_ERROR_MEMORY_MAP_FAILED";
		case VK_ERROR_LAYER_NOT_PRESENT:
			return "VK_ERROR_LAYER_NOT_PRESENT";
		case VK_ERROR_EXTENSION_NOT_PRESENT:
			return "VK_ERROR_EXTENSION_NOT_PRESENT";
		case VK_ERROR_FEATURE_NOT_PRESENT:
			return "VK_ERROR_FEATURE_NOT_PRESENT";
		case VK_ERROR_INCOMPATIBLE_DRIVER:
			return "VK_ERROR_INCOMPATIBLE_DRIVER";
		case VK_ERROR_TOO_MANY_OBJECTS:
			return "VK_ERROR_TOO_MANY_OBJECTS";
		case VK_ERROR_FORMAT_NOT_SUPPORTED:
			return "VK_ERROR_FORMAT_NOT_SUPPORTED";
		case VK_ERROR_FRAGMENTED_POOL:
			return "VK_ERROR_FRAGMENTED_POOL";
		case VK_ERROR_UNKNOWN:
			return "VK_ERROR_UNKNOWN";
		case VK_ERROR_OUT_OF_POOL_MEMORY:
			return "VK_ERROR_OUT_OF_POOL_MEMORY";
		case VK_ERROR_INVALID_EXTERNAL_HANDLE:
			return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
		case VK_ERROR_FRAGMENTATION:
			return "VK_ERROR_FRAGMENTATION";
		case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
			return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
		case VK_PIPELINE_COMPILE_REQUIRED:
			return "VK_PIPELINE_COMPILE_REQUIRED";
		case VK_ERROR_SURFACE_LOST_KHR:
			return "VK_ERROR_SURFACE_LOST_KHR";
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
			return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
		case VK_SUBOPTIMAL_KHR:
			return "VK_SUBOPTIMAL_KHR";
		case VK_ERROR_OUT_OF_DATE_KHR:
			return "VK_ERROR_OUT_OF_DATE_KHR";
		case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
			return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
		case VK_ERROR_VALIDATION_FAILED_EXT:
			return "VK_ERROR_VALIDATION_FAILED_EXT";
		case VK_ERROR_INVALID_SHADER_NV:
			return "VK_ERROR_INVALID_SHADER_NV";
#ifdef VK_ENABLE_BETA_EXTENSIONS
		case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR:
			return "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR";
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
		case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR:
			return "VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR";
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
		case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR:
			return "VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR";
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
		case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR:
			return "VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR";
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
		case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR:
			return "VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR";
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
		case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR:
			return "VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR";
#endif
		case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
			return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
		case VK_ERROR_NOT_PERMITTED_KHR:
			return "VK_ERROR_NOT_PERMITTED_KHR";
		case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
			return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
		case VK_THREAD_IDLE_KHR:
			return "VK_THREAD_IDLE_KHR";
		case VK_THREAD_DONE_KHR:
			return "VK_THREAD_DONE_KHR";
		case VK_OPERATION_DEFERRED_KHR:
			return "VK_OPERATION_DEFERRED_KHR";
		case VK_OPERATION_NOT_DEFERRED_KHR:
			return "VK_OPERATION_NOT_DEFERRED_KHR";
		case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:
			return "VK_ERROR_COMPRESSION_EXHAUSTED_EXT";
		case VK_RESULT_MAX_ENUM:
			return "VK_RESULT_MAX_ENUM";
		default:
			return "??????";
	}
}

#pragma region Debug
#if defined(WVK_DBG)
void cmd_begin_w_label(
    VkCommandBuffer cmd,
    const char     *label,
    float r, float g, float b, float a)
{
	VkDebugUtilsLabelEXT labelInfo{};
	labelInfo.sType      = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
	labelInfo.pLabelName = label;
	labelInfo.color[0]   = r;
	labelInfo.color[1]   = g;
	labelInfo.color[2]   = b;
	labelInfo.color[3]   = a;

	vkCmdBeginDebugUtilsLabelEXT(cmd, &labelInfo);
}
void cmd_end_w_label(VkCommandBuffer cmd)
{
	vkCmdEndDebugUtilsLabelEXT(cmd);
}
static void set_debug_name(VkDevice device, VkObjectType type, uint64_t handle, const char *label)
{
	VkDebugUtilsObjectNameInfoEXT nameInfo{};
	nameInfo.sType        = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	nameInfo.objectType   = type;
	nameInfo.objectHandle = handle;
	nameInfo.pObjectName  = label;

	vkSetDebugUtilsObjectNameEXT(device, &nameInfo);
}

// clang-format off
#define DEF_LABEL_FUNC(vk_type, vk_enum)                                       	   \
		void add_debug_w_label(VkDevice device, const char *label, vk_type handle) \
		{                                                                          \
			set_debug_name(device, vk_enum, WVK_RCAST(uint64_t, handle), label);   \
		}
DEF_LABEL_FUNC(VkSwapchainKHR, 			VK_OBJECT_TYPE_SWAPCHAIN_KHR)
DEF_LABEL_FUNC(VkImage, 				VK_OBJECT_TYPE_IMAGE)
DEF_LABEL_FUNC(VkImageView, 			VK_OBJECT_TYPE_IMAGE_VIEW)
DEF_LABEL_FUNC(VkShaderModule, 			VK_OBJECT_TYPE_SHADER_MODULE)
DEF_LABEL_FUNC(VkPipeline, 				VK_OBJECT_TYPE_PIPELINE)
DEF_LABEL_FUNC(VkPipelineLayout, 		VK_OBJECT_TYPE_PIPELINE_LAYOUT)
DEF_LABEL_FUNC(VkBuffer, 				VK_OBJECT_TYPE_BUFFER)
DEF_LABEL_FUNC(VkFence, 				VK_OBJECT_TYPE_FENCE)
DEF_LABEL_FUNC(VkSemaphore, 			VK_OBJECT_TYPE_SEMAPHORE)
DEF_LABEL_FUNC(VkCommandBuffer, 		VK_OBJECT_TYPE_COMMAND_BUFFER)
DEF_LABEL_FUNC(VkCommandPool, 			VK_OBJECT_TYPE_COMMAND_POOL)
DEF_LABEL_FUNC(VkDescriptorPool, 		VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT)
DEF_LABEL_FUNC(VkDescriptorSetLayout, 	VK_OBJECT_TYPE_DESCRIPTOR_POOL)
DEF_LABEL_FUNC(VkDescriptorSet, 		VK_OBJECT_TYPE_DESCRIPTOR_SET)
#undef DEF_LABEL_FUNC
// clang-format on
#endif        // WVK_DBG

#pragma endregion

#pragma region Image Utils

uint32_t bytes_per_pixel(VkFormat format)
{
	switch (format)
	{
		// Undefined
		case VK_FORMAT_UNDEFINED:
			return 0;
		// 1-byte per pixel formats
		case VK_FORMAT_R4G4_UNORM_PACK8:
		case VK_FORMAT_R8_UNORM:
		case VK_FORMAT_R8_SNORM:
		case VK_FORMAT_R8_USCALED:
		case VK_FORMAT_R8_SSCALED:
		case VK_FORMAT_R8_UINT:
		case VK_FORMAT_R8_SINT:
		case VK_FORMAT_R8_SRGB:
			return 1;

		// 2-byte per pixel formats
		case VK_FORMAT_R5G6B5_UNORM_PACK16:
		case VK_FORMAT_B5G6R5_UNORM_PACK16:
		case VK_FORMAT_R8G8_UNORM:
		case VK_FORMAT_R8G8_SNORM:
		case VK_FORMAT_R8G8_USCALED:
		case VK_FORMAT_R8G8_SSCALED:
		case VK_FORMAT_R8G8_UINT:
		case VK_FORMAT_R8G8_SINT:
		case VK_FORMAT_R8G8_SRGB:
		case VK_FORMAT_R16_UNORM:
		case VK_FORMAT_R16_SNORM:
		case VK_FORMAT_R16_USCALED:
		case VK_FORMAT_R16_SSCALED:
		case VK_FORMAT_R16_UINT:
		case VK_FORMAT_R16_SINT:
		case VK_FORMAT_R16_SFLOAT:
		case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
		case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
		case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
		case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
		case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
			return 2;

		// 3-byte per pixel formats
		case VK_FORMAT_R8G8B8_UNORM:
		case VK_FORMAT_R8G8B8_SNORM:
		case VK_FORMAT_R8G8B8_USCALED:
		case VK_FORMAT_R8G8B8_SSCALED:
		case VK_FORMAT_R8G8B8_UINT:
		case VK_FORMAT_R8G8B8_SINT:
		case VK_FORMAT_R8G8B8_SRGB:
		case VK_FORMAT_B8G8R8_UNORM:
		case VK_FORMAT_B8G8R8_SNORM:
		case VK_FORMAT_B8G8R8_USCALED:
		case VK_FORMAT_B8G8R8_SSCALED:
		case VK_FORMAT_B8G8R8_UINT:
		case VK_FORMAT_B8G8R8_SINT:
		case VK_FORMAT_B8G8R8_SRGB:
			return 3;

		// 4-byte per pixel formats
		case VK_FORMAT_R8G8B8A8_UNORM:
		case VK_FORMAT_R8G8B8A8_SNORM:
		case VK_FORMAT_R8G8B8A8_USCALED:
		case VK_FORMAT_R8G8B8A8_SSCALED:
		case VK_FORMAT_R8G8B8A8_UINT:
		case VK_FORMAT_R8G8B8A8_SINT:
		case VK_FORMAT_R8G8B8A8_SRGB:
		case VK_FORMAT_B8G8R8A8_UNORM:
		case VK_FORMAT_B8G8R8A8_SNORM:
		case VK_FORMAT_B8G8R8A8_USCALED:
		case VK_FORMAT_B8G8R8A8_SSCALED:
		case VK_FORMAT_B8G8R8A8_UINT:
		case VK_FORMAT_B8G8R8A8_SINT:
		case VK_FORMAT_B8G8R8A8_SRGB:
		case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
		case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
		case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
		case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
		case VK_FORMAT_A8B8G8R8_UINT_PACK32:
		case VK_FORMAT_A8B8G8R8_SINT_PACK32:
		case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
		case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
		case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
		case VK_FORMAT_A2R10G10B10_USCALED_PACK32:
		case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
		case VK_FORMAT_A2R10G10B10_UINT_PACK32:
		case VK_FORMAT_A2R10G10B10_SINT_PACK32:
		case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
		case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
		case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
		case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
		case VK_FORMAT_A2B10G10R10_UINT_PACK32:
		case VK_FORMAT_A2B10G10R10_SINT_PACK32:
		case VK_FORMAT_R16G16_UNORM:
		case VK_FORMAT_R16G16_SNORM:
		case VK_FORMAT_R16G16_USCALED:
		case VK_FORMAT_R16G16_SSCALED:
		case VK_FORMAT_R16G16_UINT:
		case VK_FORMAT_R16G16_SINT:
		case VK_FORMAT_R16G16_SFLOAT:
		case VK_FORMAT_R32_UINT:
		case VK_FORMAT_D24_UNORM_S8_UINT:
		case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
		case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
			return 4;

		// 6-byte formats
		case VK_FORMAT_R16G16B16_UNORM:
		case VK_FORMAT_R16G16B16_SNORM:
		case VK_FORMAT_R16G16B16_USCALED:
		case VK_FORMAT_R16G16B16_SSCALED:
		case VK_FORMAT_R16G16B16_UINT:
		case VK_FORMAT_R16G16B16_SINT:
		case VK_FORMAT_R16G16B16_SFLOAT:
			return 6;

		// 8-byte formats
		case VK_FORMAT_R32_SINT:
		case VK_FORMAT_R32_SFLOAT:
		case VK_FORMAT_R32G32_UINT:
		case VK_FORMAT_R32G32_SINT:
		case VK_FORMAT_R32G32_SFLOAT:
		case VK_FORMAT_R64_UINT:
		case VK_FORMAT_R64_SINT:
		case VK_FORMAT_R64_SFLOAT:
			return 8;

		// 12-byte formats
		case VK_FORMAT_R32G32B32_UINT:
		case VK_FORMAT_R32G32B32_SINT:
		case VK_FORMAT_R32G32B32_SFLOAT:
			return 12;

		// 16-byte formats
		case VK_FORMAT_R32G32B32A32_UINT:
		case VK_FORMAT_R32G32B32A32_SINT:
		case VK_FORMAT_R32G32B32A32_SFLOAT:
		case VK_FORMAT_R64G64_UINT:
		case VK_FORMAT_R64G64_SINT:
		case VK_FORMAT_R64G64_SFLOAT:
		case VK_FORMAT_R16G16B16A16_UNORM:
		case VK_FORMAT_R16G16B16A16_SNORM:
		case VK_FORMAT_R16G16B16A16_USCALED:
		case VK_FORMAT_R16G16B16A16_SSCALED:
		case VK_FORMAT_R16G16B16A16_UINT:
		case VK_FORMAT_R16G16B16A16_SINT:
		case VK_FORMAT_R16G16B16A16_SFLOAT:
			return 16;

		// 24-byte formats
		case VK_FORMAT_R64G64B64_UINT:
		case VK_FORMAT_R64G64B64_SINT:
		case VK_FORMAT_R64G64B64_SFLOAT:
			return 24;

		// 32-byte formats
		case VK_FORMAT_R64G64B64A64_UINT:
		case VK_FORMAT_R64G64B64A64_SINT:
		case VK_FORMAT_R64G64B64A64_SFLOAT:
			return 32;

		// Stencil-only format
		case VK_FORMAT_S8_UINT:
			return 1;

		// Depth-only format
		case VK_FORMAT_D32_SFLOAT:
			return 4;

		default:
			return 0;
	}
}

void transition_image(
    VkCommandBuffer cmd,
    VkImage         image,
    VkImageLayout   oldLayout,
    VkImageLayout   newLayout)
{
	VkImageAspectFlags aspectMask =
	    (oldLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL ||
	     newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL ||
	     newLayout == VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL) ?
	        VK_IMAGE_ASPECT_DEPTH_BIT :
	        VK_IMAGE_ASPECT_COLOR_BIT;

	const VkImageMemoryBarrier2 barrier2 =
	    init::image_memory_barrier_2(
	        VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
	        VK_ACCESS_2_MEMORY_WRITE_BIT,
	        VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
	        VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_TRANSFER_WRITE_BIT,
	        image,
	        aspectMask,
	        oldLayout,
	        newLayout);

	const VkDependencyInfo depInfo =
	    init::dependency_info({&barrier2, 1});

	vkCmdPipelineBarrier2(cmd, &depInfo);
}

void blit_image(
    VkCommandBuffer cmd,
    VkImage         src,
    VkImage         dst,
    VkExtent2D      srcExtents,
    VkExtent2D      dstExtents,
    VkFilter        filter)
{
	const VkImageBlit2 blitRegion =
	    init::image_blit_2(
	        srcExtents,
	        dstExtents);

	const VkBlitImageInfo2 blitInfo =
	    init::blit_image_info_2(
	        src,
	        dst,
	        &blitRegion,
	        filter);

	vkCmdBlitImage2(cmd, &blitInfo);
}
#pragma endregion

uint32_t to_bindless_texture_id(ImTextureID id)
{
	return WVK_CAST(uint32_t, id);
}

RenderInfo create_rendering_info(const RenderingInfo &renderingInfo)
{
	RenderInfo outRenderInfo = {};

	if (renderingInfo.colorImageView)
	{
		VkClearValue colorClearValue = renderingInfo.colorImageClearValue.value_or(VkClearValue{});
		outRenderInfo.colorAttachment =
		    init::rendering_attachment_info(
		        colorClearValue,
		        renderingInfo.colorImageView,
		        VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
		        renderingInfo.colorImageClearValue.has_value() ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
		        VK_ATTACHMENT_STORE_OP_STORE);
	}

	if (renderingInfo.depthImageView)
	{
		VkClearValue depthClearValue =
		    {.depthStencil = {
		         renderingInfo.depthImageClearValue.value_or(1.0f), 0}};
		outRenderInfo.depthAttachment =
		    init::rendering_attachment_info(
		        depthClearValue,
		        renderingInfo.depthImageView,
		        VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
		        renderingInfo.depthImageClearValue.has_value() ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_STORE);
	}

	outRenderInfo.info =
	    init::rendering_info(
	        renderingInfo.renderArea,
	        1,
	        0,
	        {&outRenderInfo.colorAttachment, 1},
	        renderingInfo.depthImageView ? &outRenderInfo.depthAttachment : nullptr);

	return outRenderInfo;
}

void clear(
    VkCommandBuffer                 cmd,
    const std::shared_ptr<Texture> &drawImage,
    const glm::vec4                &clearValue)
{
	VkClearColorValue clearColorValue = {{clearValue.x, clearValue.y, clearValue.z, clearValue.w}};

	VkImageSubresourceRange clearRange =
	    init::image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT);

	transition_image(
	    cmd,
	    drawImage->handle(),
	    VK_IMAGE_LAYOUT_UNDEFINED,
	    VK_IMAGE_LAYOUT_GENERAL);

	vkCmdClearColorImage(
	    cmd,
	    drawImage->handle(),
	    VK_IMAGE_LAYOUT_GENERAL,
	    &clearColorValue,
	    1,
	    &clearRange);

	transition_image(
	    cmd,
	    drawImage->handle(),
	    VK_IMAGE_LAYOUT_GENERAL,
	    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
}

#pragma region Shader Utils
VkShaderModule load_shader_module(VkDevice device, std::string_view filepath, std::string_view name)
{
	std::vector<uint32_t>    code             = utils::ReadShaderSpirv(filepath);
	VkShaderModuleCreateInfo shaderModuleInfo = init::shader_module_create_info(code);

	VkShaderModule outModule = VK_NULL_HANDLE;

	vk_assert(
	    vkCreateShaderModule(
	        device,
	        &shaderModuleInfo,
	        VK_NULL_HANDLE,
	        &outModule));

	add_debug_w_label(device, name.data(), outModule);
	return outModule;
}

#pragma region Pipeline Utils
VkPipelineLayout create_pipeline_layout(
    VkDevice                               device,
    std::span<const VkDescriptorSetLayout> layouts,
    std::span<const VkPushConstantRange>   pushConstantRanges,
    std::string_view                       name)
{
	const VkPipelineLayoutCreateInfo info =
	    init::pipeline_layout_create_info(
	        layouts,
	        pushConstantRanges);

	VkPipelineLayout outLayout = VK_NULL_HANDLE;

	vk_assert(
	    vkCreatePipelineLayout(
	        device,
	        &info,
	        VK_NULL_HANDLE,
	        &outLayout));

	add_debug_w_label(device, name.data(), outLayout);

	return outLayout;
}

PipelineBuilder::PipelineBuilder(VkPipelineLayout pipelineLayout)
{
	clear();
	_pipeline_layout = pipelineLayout;
}
VkPipeline PipelineBuilder::build(VkDevice device)
{
	const VkPipelineViewportStateCreateInfo viewportState =
	    init::viewport_state();

	const VkPipelineColorBlendStateCreateInfo colorBlendingState =
	    init::color_blend_state({&_color_attachment_blend, 1});

	const VkPipelineVertexInputStateCreateInfo vertexInputState =
	    init::vertex_input_state(_vertex_bindings, _vertex_attributes);

	std::vector<VkDynamicState> dynamicStates = {
	    VK_DYNAMIC_STATE_VIEWPORT,
	    VK_DYNAMIC_STATE_SCISSOR};

	const VkPipelineDynamicStateCreateInfo dynamicState =
	    init::dynamic_state(dynamicStates);

	VkGraphicsPipelineCreateInfo info = {};
	info.sType                        = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	info.pNext                        = &_render_info;
	info.stageCount                   = WVK_CAST(uint32_t, _shader_stages.size());
	info.pStages                      = _shader_stages.data();
	info.pVertexInputState            = &vertexInputState;
	info.pInputAssemblyState          = &_input_assembly;
	info.pTessellationState           = VK_NULL_HANDLE;
	info.pViewportState               = &viewportState;
	info.pRasterizationState          = &_rasterization;
	info.pMultisampleState            = &_multisample;
	info.pDepthStencilState           = &_depth_stencil;
	info.pColorBlendState             = &colorBlendingState;
	info.pDynamicState                = &dynamicState;
	info.layout                       = _pipeline_layout;

	VkPipeline outPipeline = VK_NULL_HANDLE;
	vk_assert(
	    vkCreateGraphicsPipelines(
	        device,
	        VK_NULL_HANDLE,
	        1,
	        &info,
	        VK_NULL_HANDLE,
	        &outPipeline));

	add_debug_w_label(device, _debug_name.c_str(), outPipeline);

	return outPipeline;
}
void PipelineBuilder::clear()
{
	_shader_stages.clear();

	_input_assembly                        = {};
	_input_assembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	_input_assembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	_input_assembly.primitiveRestartEnable = VK_FALSE;

	_rasterization                         = {};
	_rasterization.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	_rasterization.depthClampEnable        = VK_FALSE;
	_rasterization.rasterizerDiscardEnable = VK_FALSE;
	_rasterization.polygonMode             = VK_POLYGON_MODE_FILL;
	_rasterization.lineWidth               = 1.0f;
	_rasterization.cullMode                = VK_CULL_MODE_NONE;
	_rasterization.frontFace               = VK_FRONT_FACE_CLOCKWISE;
	_rasterization.depthBiasEnable         = VK_FALSE;

	_color_attachment_blend                = {};
	_color_attachment_blend.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	_color_attachment_blend.blendEnable    = VK_FALSE;

	_multisample                      = {};
	_multisample.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	_multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	_multisample.sampleShadingEnable  = VK_FALSE;

	_depth_stencil                       = {};
	_depth_stencil.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	_depth_stencil.depthTestEnable       = VK_TRUE;
	_depth_stencil.depthWriteEnable      = VK_TRUE;
	_depth_stencil.depthCompareOp        = VK_COMPARE_OP_LESS;
	_depth_stencil.depthBoundsTestEnable = VK_FALSE;
	_depth_stencil.stencilTestEnable     = VK_FALSE;

	_render_info                         = {};
	_render_info.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
	_render_info.colorAttachmentCount    = 0;
	_render_info.pColorAttachmentFormats = nullptr;
	_render_info.depthAttachmentFormat   = VK_FORMAT_UNDEFINED;
	_render_info.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

	_color_attachment_format = VK_FORMAT_UNDEFINED;
	_pipeline_layout         = VK_NULL_HANDLE;
	_dynamic_depth           = false;
}
PipelineBuilder &PipelineBuilder::set_name(std::string_view name)
{
	_debug_name = name;
	return *this;
}
PipelineBuilder &PipelineBuilder::set_shaders(VkShaderModule vertexShader, VkShaderModule fragmentShader)
{
	_shader_stages.clear();

	VkPipelineShaderStageCreateInfo vertStage{};
	vertStage.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertStage.stage  = VK_SHADER_STAGE_VERTEX_BIT;
	vertStage.module = vertexShader;
	vertStage.pName  = "main";

	VkPipelineShaderStageCreateInfo fragStage{};
	fragStage.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragStage.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragStage.module = fragmentShader;
	fragStage.pName  = "main";

	_shader_stages.push_back(vertStage);
	_shader_stages.push_back(fragStage);

	return *this;
}
PipelineBuilder &PipelineBuilder::set_shaders(VkShaderModule vertexShader, VkShaderModule geometryShader, VkShaderModule fragmentShader)
{
	_shader_stages.clear();

	VkPipelineShaderStageCreateInfo vertStage{};
	vertStage.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertStage.stage  = VK_SHADER_STAGE_VERTEX_BIT;
	vertStage.module = vertexShader;
	vertStage.pName  = "main";

	VkPipelineShaderStageCreateInfo geomStage{};
	geomStage.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	geomStage.stage  = VK_SHADER_STAGE_GEOMETRY_BIT;
	geomStage.module = geometryShader;
	geomStage.pName  = "main";

	VkPipelineShaderStageCreateInfo fragStage{};
	fragStage.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragStage.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragStage.module = fragmentShader;
	fragStage.pName  = "main";

	_shader_stages.push_back(vertStage);
	_shader_stages.push_back(geomStage);
	_shader_stages.push_back(fragStage);

	return *this;
}
PipelineBuilder &PipelineBuilder::set_vertex_input(std::span<const VkVertexInputBindingDescription> &bindings, std::span<const VkVertexInputAttributeDescription> &attributes)
{
	_vertex_bindings   = bindings;
	_vertex_attributes = attributes;
	return *this;
}
PipelineBuilder &PipelineBuilder::set_input_topology(VkPrimitiveTopology topology)
{
	_input_assembly.topology = topology;
	return *this;
}
PipelineBuilder &PipelineBuilder::set_polygon_mode(VkPolygonMode mode)
{
	_rasterization.polygonMode = mode;
	return *this;
}
PipelineBuilder &PipelineBuilder::set_cull_mode(VkCullModeFlags cullMode, VkFrontFace frontFace)
{
	_rasterization.cullMode  = cullMode;
	_rasterization.frontFace = frontFace;
	return *this;
}
PipelineBuilder &PipelineBuilder::enable_culling()
{
	_rasterization.cullMode = VK_CULL_MODE_BACK_BIT;
	return *this;
}
PipelineBuilder &PipelineBuilder::disable_culling()
{
	_rasterization.cullMode = VK_CULL_MODE_NONE;
	return *this;
}
PipelineBuilder &PipelineBuilder::set_multisampling_none()
{
	_multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	_multisample.sampleShadingEnable  = VK_FALSE;
	return *this;
}
PipelineBuilder &PipelineBuilder::set_multisampling(VkSampleCountFlagBits samples)
{
	_multisample.rasterizationSamples = samples;
	_multisample.sampleShadingEnable  = VK_FALSE;
	return *this;
}
PipelineBuilder &PipelineBuilder::enable_blending(VkBlendOp blendOp, VkBlendFactor srcBlendFactor, VkBlendFactor dstBlendFactor, VkBlendFactor srcAlphaBlendFactor, VkBlendFactor dstAlphaBlendFactor)
{
	_color_attachment_blend.blendEnable         = VK_TRUE;
	_color_attachment_blend.colorBlendOp        = blendOp;
	_color_attachment_blend.srcColorBlendFactor = srcBlendFactor;
	_color_attachment_blend.dstColorBlendFactor = dstBlendFactor;
	_color_attachment_blend.alphaBlendOp        = blendOp;
	_color_attachment_blend.srcAlphaBlendFactor = srcAlphaBlendFactor;
	_color_attachment_blend.dstAlphaBlendFactor = dstAlphaBlendFactor;
	return *this;
}
PipelineBuilder &PipelineBuilder::disable_blending()
{
	_color_attachment_blend.blendEnable = VK_FALSE;
	return *this;
}
PipelineBuilder &PipelineBuilder::set_color_attachment_format(VkFormat format)
{
	_color_attachment_format             = format;
	_render_info.colorAttachmentCount    = 1;
	_render_info.pColorAttachmentFormats = &_color_attachment_format;
	return *this;
}
PipelineBuilder &PipelineBuilder::set_depth_format(VkFormat format)
{
	_render_info.depthAttachmentFormat = format;
	return *this;
}
PipelineBuilder &PipelineBuilder::enable_depth(bool depthWriteEnable, VkCompareOp op)
{
	_depth_stencil.depthTestEnable  = VK_TRUE;
	_depth_stencil.depthWriteEnable = depthWriteEnable;
	_depth_stencil.depthCompareOp   = op;
	return *this;
}
PipelineBuilder &PipelineBuilder::disable_depth_test()
{
	_depth_stencil.depthTestEnable  = VK_FALSE;
	_depth_stencil.depthWriteEnable = VK_FALSE;
	return *this;
}
PipelineBuilder &PipelineBuilder::enable_depth_clamp()
{
	_rasterization.depthClampEnable = VK_TRUE;
	return *this;
}
PipelineBuilder &PipelineBuilder::enable_dynamic_depth()
{
	_dynamic_depth = true;
	return *this;
}
PipelineBuilder &PipelineBuilder::enable_depth_bias(float constantFactor, float slopeFactor)
{
	_rasterization.depthBiasEnable         = VK_TRUE;
	_rasterization.depthBiasConstantFactor = constantFactor;
	_rasterization.depthBiasSlopeFactor    = slopeFactor;
	return *this;
}
#pragma endregion
}        // namespace wvk::gfx
