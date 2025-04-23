#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <volk.h>

namespace wvk
{

using MeshId     = size_t;
using TextureId  = uint32_t;
using MaterialId = uint32_t;

static const size_t NULL_MESH_ID     = std::numeric_limits<size_t>::max();
static const size_t NULL_TEXTURE_ID  = std::numeric_limits<uint32_t>::max();
static const size_t NULL_MATERIAL_ID = std::numeric_limits<uint32_t>::max();

struct FrameData
{
	VkCommandPool   primaryPool          = VK_NULL_HANDLE;
	VkCommandBuffer primaryCommandBuffer = VK_NULL_HANDLE;
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