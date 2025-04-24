#pragma once

#include <glm/mat4x4.hpp>

#include "wvk_gfxtypes.hpp"
#include "wvk_gfxvulkan.hpp"

namespace wvk::gfx
{

class Instance;

// ---------------------------------------------------------------------------------------------
// gfx::SpriteRenderingPipeline
// ---------------------------------------------------------------------------------------------
#pragma region gfx::SpriteRenderingPipeline
class WVK_API  SpriteRenderingPipeline final
{
  public:
	void init(Instance &instance, VkFormat drawImageFormat, size_t maxSprites, std::string_view name = "");
	void cleanup(VkDevice device);
	void draw(VkCommandBuffer cmd, Instance &instance, const std::shared_ptr<Texture> &drawImage, const glm::mat4 &viewProj, const std::vector<SpriteDrawCommand> &drawCommands);

  private:
	struct PushConstants
	{
		glm::mat4       view_proj;
		VkDeviceAddress commandBuffer = 0;
	};

	struct PerframeData
	{
		Buffer sprite_draw_command_buffer;
	};

	std::string _debug_name;

	VkPipelineLayout _layout   = VK_NULL_HANDLE;
	VkPipeline       _pipeline = VK_NULL_HANDLE;

	std::array<PerframeData, FRAME_OVERLAP> _frames;

	PerframeData &get_current_frame_data(uint32_t frameIndex);

  private:
	void _copy_buffers(VkCommandBuffer cmd, Instance &instance) const;
};

// ---------------------------------------------------------------------------------------------
// gfx::ImGuiRenderingPipeline
// ---------------------------------------------------------------------------------------------
class WVK_API ImGuiRenderingPipeline final
{
  public:
	void init(Instance &instance, VkFormat drawImageFormat, std::string_view name = "");
	void cleanup(VkDevice device);
	void draw(VkCommandBuffer cmd, Instance &instance, VkImageView imageView, VkExtent2D extent);

  private:
	struct PushConstants
	{
		VkDeviceAddress vertexBuffer;
		uint32_t        textureId;
		uint32_t        textureIsSRGB;
		glm::vec2       translate;
		glm::vec2       scale;
	};

	std::string _debug_name;

	NBuffer _indexBuffer;
	NBuffer _vertexBuffer;

	TextureId _font_texture_id = NULL_TEXTURE_ID;

	VkPipelineLayout _layout   = VK_NULL_HANDLE;
	VkPipeline       _pipeline = VK_NULL_HANDLE;

  private:
	void _copy_buffers(VkCommandBuffer cmd, Instance &instance);
};
#pragma endregion
}        // namespace wvk::gfx
