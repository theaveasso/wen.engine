#include "wvk_gfxpipeline.hpp"

#include <backends/imgui_impl_vulkan.h>
#include <glm/gtx/string_cast.hpp>
#include <imgui.h>

#include "wvk_gfx.hpp"
#include "wvk_gfxinst.hpp"
#include "wvk_gfxtypes.hpp"
#include "wvk_gfxutils.hpp"
#include "wvk_gfxvkinit.hpp"
#include "wvk_gfxvulkan.hpp"

namespace wvk::gfx
{

// ---------------------------------------------------------------------------------------------
// gfx::SpriteRenderingPipeline
// ---------------------------------------------------------------------------------------------
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
		_frames[i].sprite_draw_command_buffer.cleanup();
	}
	vkDestroyPipelineLayout(device, _layout, VK_NULL_HANDLE);
	vkDestroyPipeline(device, _pipeline, VK_NULL_HANDLE);
}
void SpriteRenderingPipeline::draw(
    VkCommandBuffer                            cmd,
    Instance                                  &instance,
    const std::shared_ptr<Texture>            &drawImage,
    const glm::mat4                           &viewProj,
    const std::vector<wvk::SpriteDrawCommand> &drawCommands)
{
	if (drawCommands.empty())
	{
		return;
	}

	const Buffer &pfd = get_current_frame_data(instance.get_current_frame_index()).sprite_draw_command_buffer;
	pfd.copy_data_to_buffer(drawCommands.data(), drawCommands.size() * sizeof(wvk::SpriteDrawCommand));

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
	    .commandBuffer = instance.get_buffer_address(pfd)};

	vkCmdPushConstants(cmd,
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

// ---------------------------------------------------------------------------------------------
// gfx::ImGuiRenderingPipeline
// ---------------------------------------------------------------------------------------------
void ImGuiRenderingPipeline::init(
    Instance        &instance,
    VkFormat         drawImageFormat,
    std::string_view name)
{
	_indexBuffer = instance.create_nbuffer(
	    instance,
	    sizeof(ImDrawIdx) * IMGUI_MAX_INDEX_COUNT,
	    VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
	    std::string(name) + "idx buffer");

	_vertexBuffer = instance.create_nbuffer(
	    instance,
	    sizeof(ImDrawVert) * IMGUI_MAX_VERTEX_COUNT,
	    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
	    std::string(name) + "imgui vertex buffer");

	{        // upload font
		ImGuiIO &io = ImGui::GetIO();

		ImFontConfig cfg         = ImFontConfig();
		cfg.FontDataOwnedByAtlas = false;
		cfg.RasterizerMultiply   = 1.5f;
		cfg.SizePixels           = std::ceilf(16);
		cfg.PixelSnapH           = true;
		cfg.OversampleH          = 4;
		cfg.OversampleV          = 4;
		ImFont *font             = nullptr;

		std::string fontPath = (std::filesystem::current_path() / "etc/data/OpenSans-Light.ttf").string();
		io.Fonts->AddFontFromFileTTF(fontPath.c_str(), cfg.SizePixels, &cfg);

		unsigned char *pixels = WVK_CAST(uint8_t *, nullptr);
		int            width  = 0;
		int            height = 0;

		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

		io.Fonts->Flags |= ImFontAtlasFlags_NoPowerOfTwoHeight;

		_font_texture_id = instance.create_texture_w_pixels(
		    VK_FORMAT_R8G8B8A8_UNORM,
		    VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		    VkExtent2D{WVK_CAST(uint32_t, width), WVK_CAST(uint32_t, height)},
		    pixels,
		    "imgui font texture");

		io.Fonts->SetTexID(WVK_CAST(ImTextureID, _font_texture_id));
		io.FontDefault = font;
	}

	{        // init pipeline
		const VkShaderModule vertShader = load_shader_module(instance.get_device(), "Builtin.ImGui.vert.glsl", "imgui vertex shader");
		const VkShaderModule fragShader = load_shader_module(instance.get_device(), "Builtin.ImGui.frag.glsl", "imgui fragment shader");

		VkPushConstantRange pushConstant = {};
		pushConstant.stageFlags          = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstant.size                = sizeof(PushConstants);
		pushConstant.offset              = 0;

		VkDescriptorSetLayout setLayout = instance.get_bindless_desc_set_layout();

		_layout = create_pipeline_layout(instance.get_device(), {&setLayout, 1}, {&pushConstant, 1}, "imgui pipeline layout");

		_pipeline =
		    PipelineBuilder(_layout)
		        .set_name("imgui pipeline")
		        .set_shaders(vertShader, fragShader)
		        .set_input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
		        .set_polygon_mode(VK_POLYGON_MODE_FILL)
		        .disable_culling()
		        .set_multisampling_none()
		        .enable_blending(
		            VK_BLEND_OP_ADD,
		            VK_BLEND_FACTOR_ONE,
		            VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
		            VK_BLEND_FACTOR_ONE,
		            VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA)
		        .set_color_attachment_format(drawImageFormat)
		        .disable_depth_test()
		        .build(instance.get_device());

		vkDestroyShaderModule(instance.get_device(), vertShader, VK_NULL_HANDLE);
		vkDestroyShaderModule(instance.get_device(), fragShader, VK_NULL_HANDLE);
	}
}

void ImGuiRenderingPipeline::cleanup(
    VkDevice device)
{
	vkDestroyPipelineLayout(device, _layout, VK_NULL_HANDLE);
	vkDestroyPipeline(device, _pipeline, VK_NULL_HANDLE);

	_indexBuffer.cleanup(device);
	_vertexBuffer.cleanup(device);
}

void ImGuiRenderingPipeline::draw(
    VkCommandBuffer cmd,
    Instance       &instance,
    VkImageView     imageView,
    VkExtent2D      extent)
{
	const ImDrawData *drawData = ImGui::GetDrawData();

	if (drawData->TotalVtxCount == 0)
	{
		return;
	}

	if (drawData->TotalIdxCount > IMGUI_MAX_INDEX_COUNT || drawData->TotalVtxCount > IMGUI_MAX_VERTEX_COUNT)
	{
		WVK_WARN("ImGuiRenderer: too many vertices/indices to render (max indices {}, max vertices {}) buffer resize is not yet implemented.", IMGUI_MAX_INDEX_COUNT, IMGUI_MAX_VERTEX_COUNT);
		WVK_ASSERT_MSG(false, "implement ImGui buffer resize!");
		return;
	}

	_copy_buffers(cmd, instance);

	const RenderInfo renderInfo = create_rendering_info(
	    {.renderArea     = {{0}, extent},
	     .colorImageView = imageView});

	vkCmdBeginRendering(cmd, &renderInfo.info);

	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);
	instance.bind_bindless_desc_set(cmd, _layout);

	float targetWidth  = WVK_CAST(float, extent.width);
	float targetHeight = WVK_CAST(float, extent.height);

	VkViewport viewport = {};
	viewport.x          = 0;
	viewport.y          = 0;
	viewport.width      = targetWidth;
	viewport.height     = targetHeight;
	viewport.minDepth   = 0.0f;
	viewport.maxDepth   = 1.0f;

	vkCmdSetViewport(cmd, 0, 1, &viewport);

	const ImVec2 clipOffset = drawData->DisplayPos;
	const ImVec2 clipScale  = drawData->FramebufferScale;

	size_t globalIdxOffset = 0;
	size_t globalVtxOffset = 0;

	vkCmdBindIndexBuffer(
	    cmd,
	    _indexBuffer.handle().handle(),
	    0,
	    sizeof(ImDrawIdx) == sizeof(uint16_t) ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);

	for (int cmdListId = 0; cmdListId < drawData->CmdListsCount; ++cmdListId)
	{
		const ImDrawList &cmdList = *drawData->CmdLists[cmdListId];
		for (int cmdId = 0; cmdId < cmdList.CmdBuffer.Size; ++cmdId)
		{
			const auto &imCmd = cmdList.CmdBuffer[cmdId];
			if (imCmd.UserCallback)
			{
				if (imCmd.UserCallback != ImDrawCallback_ResetRenderState)
				{
					imCmd.UserCallback(&cmdList, &imCmd);
					continue;
				}
				WVK_ASSERT_MSG(false, "ImDrawCallback_ResetRenderState is not supported!");
			}

			if (imCmd.ElemCount == 0)
			{
				continue;
			}

			// project scissor/clipping rectangles into framebuffer space.
			ImVec2 clipMin((imCmd.ClipRect.x - clipOffset.x) * clipScale.x, (imCmd.ClipRect.y - clipOffset.y) * clipScale.y);
			ImVec2 clipMax((imCmd.ClipRect.z - clipOffset.x) * clipScale.x, (imCmd.ClipRect.w - clipOffset.y) * clipScale.y);

			// clamp to viewport as vkCmdSetScissor() won't accept values that are off bounds.
			clipMin.x = std::clamp(clipMin.x, 0.0f, targetWidth);
			clipMax.x = std::clamp(clipMax.x, 0.0f, targetWidth);
			clipMin.y = std::clamp(clipMin.y, 0.0f, targetHeight);
			clipMax.y = std::clamp(clipMax.y, 0.0f, targetHeight);
			if (clipMax.x <= clipMin.x || clipMax.y <= clipMin.y)
			{
				continue;
			}

			// apply scissor/clipping rectangle.
			VkRect2D scissor = {};
			scissor.offset   = {WVK_CAST(int32_t, clipMin.x), WVK_CAST(int32_t, clipMin.y)};
			scissor.extent   = {WVK_CAST(uint32_t, clipMax.x - clipMin.x), WVK_CAST(uint32_t, clipMax.y - clipMin.y)};
			vkCmdSetScissor(cmd, 0, 1, &scissor);

			// Bind descriptor set with font or user texture.
			TextureId textureId = instance.get_white_texture_id();
			if (imCmd.TextureId != ImTextureID())
			{
				textureId = to_bindless_texture_id(imCmd.TextureId);
				WVK_ASSERT_MSG(textureId != NULL_TEXTURE_ID, "");
			}

			bool                      textureIsSRGB = true;
			const shared_ptr<Texture> texture       = instance.get_texture(textureId);
			if (texture->format() == VK_FORMAT_R8G8B8A8_SRGB || texture->format() == VK_FORMAT_R16G16B16A16_SFLOAT)
			{
				textureIsSRGB = false;
			}

			const glm::vec2 scale     = {2.0f / drawData->DisplaySize.x, 2.0f / drawData->DisplaySize.y};
			const glm::vec2 translate = {-1.0f - drawData->DisplayPos.x * scale.x, -1.0f - drawData->DisplayPos.y * scale.y};

			const PushConstants pcs = {
			    .vertexBuffer  = instance.get_buffer_address(_vertexBuffer.handle()),
			    .textureId     = WVK_CAST(uint32_t, textureId),
			    .textureIsSRGB = textureIsSRGB,
			    .translate     = translate,
			    .scale         = scale};

			vkCmdPushConstants(
			    cmd,
			    _layout,
			    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			    0,
			    sizeof(PushConstants),
			    &pcs);

			vkCmdDrawIndexed(
			    cmd,
			    imCmd.ElemCount,
			    1,
			    imCmd.IdxOffset + WVK_CAST(uint32_t, globalIdxOffset),
			    imCmd.VtxOffset + WVK_CAST(uint32_t, globalVtxOffset),
			    0);
		}

		globalIdxOffset += cmdList.IdxBuffer.Size;
		globalVtxOffset += cmdList.VtxBuffer.Size;
	}

	vkCmdEndRendering(cmd);
}

void ImGuiRenderingPipeline::_copy_buffers(
    VkCommandBuffer cmd,
    Instance       &instance)
{
	const ImDrawData *drawData = ImGui::GetDrawData();
	if (!drawData || drawData->CmdListsCount == 0)
	{
		return;
	}

	{        // sync with previous read
		const VkBufferMemoryBarrier2 indexBarrier =
		    init::buffer_memory_barrier2(
		        VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
		        VK_ACCESS_2_MEMORY_READ_BIT,
		        VK_PIPELINE_STAGE_2_TRANSFER_BIT,
		        VK_ACCESS_2_MEMORY_WRITE_BIT,
		        _indexBuffer.handle().handle());

		const VkBufferMemoryBarrier2 vertexBarrier =
		    init::buffer_memory_barrier2(
		        VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
		        VK_ACCESS_2_MEMORY_READ_BIT,
		        VK_PIPELINE_STAGE_2_TRANSFER_BIT,
		        VK_ACCESS_2_MEMORY_WRITE_BIT,
		        _vertexBuffer.handle().handle());

		std::array<VkBufferMemoryBarrier2, 2> barrier = {indexBarrier, vertexBarrier};

		const VkDependencyInfo depInfo =
		    init::dependency_info({}, barrier);

		vkCmdPipelineBarrier2(cmd, &depInfo);
	}
	const uint32_t frameIndex       = instance.get_current_frame_index();
	size_t         currIndexOffset  = 0;
	size_t         currVertexOffset = 0;

	size_t totalIdxCount = 0;
	size_t totalVtxCount = 0;

	for (int i = 0; i < drawData->CmdListsCount; ++i)
	{
		totalIdxCount += drawData->CmdLists[i]->IdxBuffer.size();
		totalVtxCount += drawData->CmdLists[i]->VtxBuffer.size();
	}

	WVK_ASSERT_MSG(sizeof(ImDrawVert) * totalVtxCount <= _vertexBuffer.size(), "");
	WVK_ASSERT_MSG(sizeof(ImDrawIdx) * totalIdxCount <= _indexBuffer.size(), "");

	for (size_t i = 0; i < drawData->CmdListsCount; ++i)
	{
		const ImDrawList &cmdList = *drawData->CmdLists[i];

		_indexBuffer.upload(
		    cmd,
		    frameIndex,
		    cmdList.IdxBuffer.Data,
		    sizeof(ImDrawIdx) * cmdList.IdxBuffer.Size,
		    sizeof(ImDrawIdx) * currIndexOffset,
		    false);

		_vertexBuffer.upload(
		    cmd,
		    frameIndex,
		    cmdList.VtxBuffer.Data,
		    sizeof(ImDrawVert) * cmdList.VtxBuffer.Size,
		    sizeof(ImDrawVert) * currVertexOffset,
		    false);

		currIndexOffset += cmdList.IdxBuffer.Size;
		currVertexOffset += cmdList.VtxBuffer.Size;
	}
	{        // sync with previous read
		VkBufferMemoryBarrier2 indexBarrier =
		    init::buffer_memory_barrier2(
		        VK_PIPELINE_STAGE_2_COPY_BIT,
		        VK_ACCESS_2_MEMORY_WRITE_BIT,
		        VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
		        VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT,
		        _indexBuffer.handle().handle());

		const VkBufferMemoryBarrier2 vertexBarrier =
		    init::buffer_memory_barrier2(
		        VK_PIPELINE_STAGE_2_COPY_BIT,
		        VK_ACCESS_2_MEMORY_WRITE_BIT,
		        VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
		        VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT,
		        _vertexBuffer.handle().handle());

		std::array<VkBufferMemoryBarrier2, 2> barrier = {indexBarrier, vertexBarrier};

		const VkDependencyInfo depInfo =
		    init::dependency_info({}, barrier);

		vkCmdPipelineBarrier2(cmd, &depInfo);
	}
}
}        // namespace wvk::gfx
