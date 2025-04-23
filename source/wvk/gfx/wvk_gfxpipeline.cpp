#include "wvk_gfxpipeline.hpp"

#include "wvk_gfx.hpp"
#include "wvk_gfxtypes.hpp"
#include "wvk_gfxutils.hpp"

namespace wvk::gfx
{
#pragma region gfx::SpriteRenderingPipeline
void           SpriteRenderingPipeline::init(
    Instance        &instance,
    VkFormat         drawImageFormat,
    size_t           maxSprites,
    std::string_view name)
{
	_debug_name = name;

	VkShaderModule vertShader = load_shader_module(instance.get_device(), "Builtin.Sprite.vert.glsl", "sprite vertex shader");
	VkShaderModule fragShader = load_shader_module(instance.get_device(), "Builtin.Sprite.frag.glsl", "sprite fragment shader");

	VkPushConstantRange pushConstant = {};
	pushConstant.stageFlags          = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstant.size                = sizeof(PushConstants);
	pushConstant.offset              = 0;

	VkDescriptorSetLayout setLayout = instance.get_bindless_desc_set_layout();

	_layout = create_pipeline_layout(instance.get_device(), {&setLayout, 1}, {&pushConstant, 1}, "sprite pipeline layout");

	_pipeline =
	    PipelineBuilder(_layout)
	        .set_name("ui pipeline")
	        .set_shaders(vertShader, fragShader)
	        .set_input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
	        .set_polygon_mode(VK_POLYGON_MODE_FILL)
	        .disable_culling()
	        .set_multisampling_none()
	        .set_color_attachment_format(drawImageFormat)
	        .disable_depth_test()
	        .build(instance.get_device());

	vkDestroyShaderModule(instance.get_device(), vertShader, VK_NULL_HANDLE);
	vkDestroyShaderModule(instance.get_device(), fragShader, VK_NULL_HANDLE);

	for (size_t i = 0; i < FRAME_OVERLAP; ++i)
	{
		_frames[i].sprite_draw_command_buffer = instance.create_persistent_buffer(
		    maxSprites * sizeof(wvk::SpriteDrawCommand),
		    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		    "sprite draw commands");
	}
}
void SpriteRenderingPipeline::cleanup(VkDevice device)
{
	for (size_t i = 0; i < FRAME_OVERLAP; ++i)
	{
		_frames[i].sprite_draw_command_buffer->cleanup();
	}
	vkDestroyPipelineLayout(device, _layout, VK_NULL_HANDLE);
	vkDestroyPipeline(device, _pipeline, VK_NULL_HANDLE);
}
void SpriteRenderingPipeline::draw(
    VkCommandBuffer                       cmd,
    Instance                             &instance,
    const std::shared_ptr<Texture>       &drawImage,
    const glm::mat4                      &viewProj,
    const std::vector<wvk::SpriteDrawCommand> &drawCommands)
{
	if (drawCommands.empty())
	{
		return;
	}

	const std::shared_ptr<Buffer> &pfd = get_current_frame_data(instance.get_current_frame_index()).sprite_draw_command_buffer;
	pfd->copy_data_to_buffer(drawCommands.data(), drawCommands.size() * sizeof(wvk::SpriteDrawCommand));

	const RenderInfo renderInfo = create_rendering_info(
	    {.renderArea     = {{0}, drawImage->extent()},
	     .colorImageView = drawImage->image_view()});

	vkCmdBeginRendering(cmd, &renderInfo.info);

	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);

	VkViewport viewport = {};
	viewport.x          = 0;
	viewport.y          = 0;
	viewport.width      = WVK_CAST(float, drawImage->extent().width);
	viewport.height     = WVK_CAST(float, drawImage->extent().height);
	viewport.minDepth   = 0;
	viewport.maxDepth   = 1.f;

	VkRect2D scissor = {};
	scissor.offset   = {};
	scissor.extent   = drawImage->extent();

	vkCmdSetViewport(cmd, 0, 1, &viewport);
	vkCmdSetScissor(cmd, 0, 1, &scissor);

	instance.bind_bindless_desc_set(cmd, _layout);

	const PushConstants pushConstants = {
	    .view_proj     = viewProj,
	    .commandBuffer = pfd->device_address(instance.get_device())};

	vkCmdPushConstants(
	    cmd,
	    _layout,
	    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
	    0,
	    sizeof(PushConstants),
	    &pushConstants);

	vkCmdDraw(cmd, 6, WVK_CAST(uint32_t, drawCommands.size()), 0, 0);

	vkCmdEndRendering(cmd);
}

SpriteRenderingPipeline::PerframeData &SpriteRenderingPipeline::get_current_frame_data(
    uint32_t frameIndex)
{
	return _frames[frameIndex];
}
#pragma endregion
}        // namespace wvk::gfx