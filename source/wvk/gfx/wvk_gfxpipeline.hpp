#pragma once

#include "wvk_gfxvulkan.hpp"
#include <glm/mat4x4.hpp>

namespace wvk
{
struct SpriteDrawCommand;
}

namespace wvk::gfx
{
class Instance;

#pragma region gfx::SpriteRenderingPipeline
class WVK_API  SpriteRenderingPipeline final
{
  public:
	void init(Instance &instance, VkFormat drawImageFormat, size_t maxSprites, std::string_view name = "");
	void cleanup(VkDevice device);

	void draw(VkCommandBuffer cmd, Instance &instance, const std::shared_ptr<Texture> &drawImage, const glm::mat4 &viewProj, const std::vector<SpriteDrawCommand> &drawCommands);

  private:
	std::string _debug_name;

	VkPipelineLayout _layout   = VK_NULL_HANDLE;
	VkPipeline       _pipeline = VK_NULL_HANDLE;

	struct PushConstants
	{
		glm::mat4       view_proj;
		VkDeviceAddress commandBuffer = 0;
	};

	struct PerframeData
	{
		std::shared_ptr<Buffer> sprite_draw_command_buffer = nullptr;
	};

	std::array<PerframeData, FRAME_OVERLAP> _frames;

	PerframeData &get_current_frame_data(uint32_t frameIndex);
};
#pragma endregion
}        // namespace wvk::gfx