#pragma once

// clang-format off
#include <vulkan/vulkan.h>
#include <volk.h>
#include <tracy/TracyVulkan.hpp>
// clang-format on

#ifdef TRACY_ENABLE
#	define WVK_PROFILER_GPU_ZONE(name, ctx, cmd, color) \
		TracyVkZoneC(ctx, cmd, name, color)
#else
#	define WVK_PROFILER_GPU_ZONE(name, ctx, cmd, color) (void) 0
#endif

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <imgui.h>

namespace core
{
class Camera;
}

namespace wvk
{

using MeshId     = size_t;
using TextureId  = uint32_t;
using MaterialId = uint32_t;

static const size_t NULL_MESH_ID     = std::numeric_limits<size_t>::max();
static const size_t NULL_TEXTURE_ID  = std::numeric_limits<uint32_t>::max();
static const size_t NULL_MATERIAL_ID = std::numeric_limits<uint32_t>::max();

static constexpr uint32_t IMGUI_MAX_INDEX_COUNT  = 1000000;
static constexpr uint32_t IMGUI_MAX_VERTEX_COUNT = 1000000;

struct RenderingInfo
{
	VkRect2D                    renderArea           = {};
	VkImageView                 colorImageView       = VK_NULL_HANDLE;
	VkImageView                 depthImageView       = VK_NULL_HANDLE;
	VkImageView                 stencilImageView     = VK_NULL_HANDLE;
	std::optional<float>        depthImageClearValue = std::nullopt;
	std::optional<VkClearValue> colorImageClearValue = std::nullopt;
};

struct RenderInfo
{
	VkRenderingAttachmentInfo colorAttachment   = {};
	VkRenderingAttachmentInfo depthAttachment   = {};
	VkRenderingAttachmentInfo stencilAttachment = {};
	VkRenderingInfo           info              = {};
};

struct CreateTextureInfo
{
	VkFormat           format    = VK_FORMAT_B8G8R8A8_SRGB;
	VkImageUsageFlags  usage     = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	VkImageCreateFlags flags     = 0;
	VkExtent3D         extent    = {};
	uint32_t           numLayers = 1;
	VkSampleCountFlags sampler   = VK_SAMPLE_COUNT_1_BIT;
	VkImageTiling      tiling    = VK_IMAGE_TILING_OPTIMAL;
	bool               mipMap    = false;
	bool               isCubeMap = false;
};

struct FrameData
{
	VkCommandPool   primaryPool          = VK_NULL_HANDLE;
	VkCommandBuffer primaryCommandBuffer = VK_NULL_HANDLE;
	TracyVkCtx      tracyVkCtx;
};

struct SpriteDrawCommand
{
	glm::mat4 transform;
	glm::vec2 uv0;
	glm::vec2 uv1;
	glm::vec4 color;
	uint32_t  textureId;
	uint32_t  shaderId;
	glm::vec2 padding;
};

struct ImGuiVertexFormat
{
	ImVec2 position;
	ImVec2 uv;
	ImU32  color;
};

struct MeshDrawCommand
{
	MeshId     meshId;
	MaterialId materialId = NULL_MATERIAL_ID;
};

struct EndFrameParams
{
	const glm::vec4 clearColor             = {0.0f, 1.0f, 1.0f, 1.0f};
	bool            blitImageIntoSwapchain = true;
	glm::ivec4      drawImageBlitRect      = {};
	bool            drawImageLinerBlit     = false;
	bool            drawImGui              = false;
};

}        // namespace wvk