#include "vk_renderer.hpp"

#include <GLFW/glfw3.h>
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>

#include <vk_mem_alloc.h>

#include "graphics/renderer/vulkan/common/vk_common.hpp"
#include "vk_options.hpp"
#include "vk_sync.hpp"
#include "vk_utils.hpp"

using namespace std::chrono;

static WenVkRenderer vk_renderer;

static void resize();
static void update_current_frame_index();
static void set_renderer_state(VkCommandBuffer command_buffer);

static void allocator_init();
static void allocator_fini();

static void render_images_init();
static void render_images_fini();
static void configure_render_resources();

static void frames_init();
static void frames_fini();

static void command_buffers_init();
static void command_buffers_fini();

bool vk_renderer_init(
    WenRendererBackend *,
    WenWindow  &window,
    const char *app_name,
    int32_t     width,
    int32_t     height)
{
	vk_context_init(&vk_renderer.vk_ctx, window, app_name);

	vk_swapchain_ctx_init(&vk_renderer.swapchain_context, &vk_renderer.vk_ctx);

	vk_renderer.image_extent.width            = width;
	vk_renderer.image_extent.height           = height;
	vk_renderer.render_area.bottom_left       = {0, height};
	vk_renderer.render_area.scissor_dimension = {width, height};

	allocator_init();

	frames_init();

	command_buffers_init();

	render_images_init();

	configure_render_resources();

	// initialize buffers vertex, index buffer.
	vk_renderer.vertex_buffer = wen_vk_buffer_init(
	    &vk_renderer.vk_ctx, sizeof(vertices[0]) * vertices.size(),
	    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
	    true);
	vk_renderer.index_buffer = wen_vk_buffer_init(
	    &vk_renderer.vk_ctx, sizeof(uint32_t) * indices.size(),
	    VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
	    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
	    true);

	// initialize shader module, pipeline layout, pipeline.
	wen_vk_shader_object_init(
	    vk_renderer.vk_ctx.logical_device,
	    vk_renderer.swapchain_context.format,
	    &vk_renderer.vk_object_shader);

	return true;
}

void vk_renderer_fini(WenRendererBackend *)
{
	vkDeviceWaitIdle(vk_renderer.vk_ctx.logical_device);

	// Destroy buffers and free device memory
	wen_vk_buffer_fini(
	    &vk_renderer.vk_ctx,
	    &vk_renderer.vertex_buffer);
	wen_vk_buffer_fini(
	    &vk_renderer.vk_ctx,
	    &vk_renderer.index_buffer);

	// Destroy shader modules, pipeline layout, pipeline
	wen_vk_shader_object_fini(
	    vk_renderer.vk_ctx.logical_device,
	    &vk_renderer.vk_object_shader);

	render_images_fini();
	command_buffers_fini();
	frames_fini();
	vk_swapchain_ctx_fini(&vk_renderer.swapchain_context, &vk_renderer.vk_ctx);
	allocator_fini();
	vk_context_fini(&vk_renderer.vk_ctx);
}

void vk_renderer_resize(WenRendererBackend *, uint32_t, uint32_t)
{
	vkDeviceWaitIdle(vk_renderer.vk_ctx.logical_device);

	vk_swapchain_ctx_reset(&vk_renderer.swapchain_context, &vk_renderer.vk_ctx);

	vk_allocated_image_fini(
	    &vk_renderer.vk_ctx, vk_renderer.allocator, &vk_renderer.color_image);
	vk_allocated_image_fini(
	    &vk_renderer.vk_ctx, vk_renderer.allocator, &vk_renderer.depth_image);

	vk_renderer.image_extent.width  = vk_renderer.swapchain_context.extent.width;
	vk_renderer.image_extent.height = vk_renderer.swapchain_context.extent.height;
}

bool vk_renderer_begin_frame(WenRendererBackend *, float)
{
	return true;
}

bool vk_renderer_end_frame(WenRendererBackend *, float)
{
	auto start_frame_time = system_clock::now();

	update_current_frame_index();
	WenVkFrameBuffer *current_frame = &vk_renderer.frames[vk_renderer.current_frame_index];
	VkCommandBuffer   cmd_buf       = current_frame->command_context.primary_buffer;

	vkWaitForFences(
	    vk_renderer.vk_ctx.logical_device,
	    1,
	    &current_frame->render_fence,
	    VK_TRUE,
	    vk_opts::timeout_ns);

	uint32_t swapchain_image_index;
	VkResult result = vkAcquireNextImageKHR(
	    vk_renderer.vk_ctx.logical_device,
	    vk_renderer.swapchain_context.swapchain,
	    vk_opts::timeout_ns,
	    current_frame->present_semaphore,
	    nullptr,
	    &swapchain_image_index);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		resize();
		return false;
	}

	vk_check(vkResetFences(
	    vk_renderer.vk_ctx.logical_device,
	    1,
	    &current_frame->render_fence));

	VkImage swapchain_image = vk_renderer.swapchain_context.images[swapchain_image_index];
	vk_command_ctx_begin_primary_buffer(
	    &current_frame->command_context,
	    VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	vk_image_memory_barrier_insert(
	    cmd_buf,
	    vk_renderer.images[vk_renderer.current_frame_index].image,
	    VK_IMAGE_LAYOUT_UNDEFINED,
	    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	VkClearValue entity_id_clear_value = {.color = {{1, 0.2, 0, 0}}};

	VkImageSubresourceRange range =
	    vk_image_subresource_range_init(
	        VK_IMAGE_ASPECT_COLOR_BIT,
	        1,
	        1,
	        0);

	vkCmdClearColorImage(
	    cmd_buf,
	    vk_renderer.images[vk_renderer.current_frame_index].image,
	    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
	    &entity_id_clear_value.color,
	    1,
	    &range);

	vk_image_memory_barrier_insert(
	    cmd_buf,
	    vk_renderer.images[vk_renderer.current_frame_index].image,
	    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
	    VK_IMAGE_LAYOUT_GENERAL);

	vk_image_memory_barrier_insert(
	    cmd_buf,
	    vk_renderer.color_image.image,
	    VK_IMAGE_LAYOUT_UNDEFINED,
	    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	vk_image_memory_barrier_insert(
	    cmd_buf,
	    vk_renderer.depth_image.image,
	    VK_IMAGE_LAYOUT_UNDEFINED,
	    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	vkCmdBeginRendering(
	    cmd_buf,
	    &vk_renderer.scene_rendering_info);

	vkCmdBindPipeline(
	    cmd_buf,
	    VK_PIPELINE_BIND_POINT_GRAPHICS,
	    vk_renderer.vk_object_shader.pipeline_info.pipeline);

	set_renderer_state(cmd_buf);

	VkDeviceSize offset = {0};
	vkCmdBindVertexBuffers(cmd_buf,
	                       0,
	                       1,
	                       &vk_renderer.vertex_buffer.handle,
	                       &offset);
	vkCmdBindIndexBuffer(cmd_buf,
	                     vk_renderer.index_buffer.handle,
	                     offset,
	                     VK_INDEX_TYPE_UINT32);

	vkCmdDrawIndexed(
	    cmd_buf,
	    6,
	    1,
	    0,
	    0,
	    0);

	vkCmdEndRendering(cmd_buf);

	vk_image_memory_barrier_insert(
	    cmd_buf,
	    vk_renderer.color_image.image,
	    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	    VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

	vk_image_memory_barrier_insert(
	    cmd_buf,
	    swapchain_image,
	    VK_IMAGE_LAYOUT_UNDEFINED,
	    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	vk_image_blit(
	    cmd_buf,
	    vk_renderer.color_image.image,
	    swapchain_image,
	    vk_renderer.swapchain_context.extent,
	    vk_renderer.image_extent);

	vk_image_memory_barrier_insert(
	    cmd_buf,
	    swapchain_image,
	    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
	    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	VkSemaphoreSubmitInfo wait_semaphore_submit_info =
	    vk_semaphore_submit_info_ini(
	        current_frame->present_semaphore,
	        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);

	VkSemaphoreSubmitInfo signal_semaphore_submit_info =
	    vk_semaphore_submit_info_ini(
	        current_frame->render_semaphore,
	        VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT);

	vk_command_ctx_submit_primary_buffer(
	    &current_frame->command_context,
	    vk_renderer.vk_ctx.queues.graphics,
	    current_frame->render_fence,
	    &wait_semaphore_submit_info,
	    &signal_semaphore_submit_info);

	VkPresentInfoKHR present_info   = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
	present_info.swapchainCount     = 1;
	present_info.pSwapchains        = &vk_renderer.swapchain_context.swapchain;
	present_info.pWaitSemaphores    = &current_frame->render_semaphore;
	present_info.pImageIndices      = &swapchain_image_index;
	present_info.waitSemaphoreCount = 1;

	result =
	    vkQueuePresentKHR(vk_renderer.vk_ctx.queues.present, &present_info);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		resize();
		return false;
	}

	vk_renderer.frame_number++;

	auto end_time = system_clock::now();
	// auto dur      = duration<float>(end_time - start_frame_time);

	return true;
}

VkRenderingInfo vk_rendering_info_init(
    std::span<VkRenderingAttachmentInfo *> color_attachments,
    const VkRenderingAttachmentInfo       *depth_attachment,
    VkExtent2D                             extent)
{
	VkRenderingInfo rendering_info      = {VK_STRUCTURE_TYPE_RENDERING_INFO};
	rendering_info.renderArea           = VkRect2D{.offset = VkOffset2D{0, 0}, .extent = extent};
	rendering_info.colorAttachmentCount = color_attachments.size();
	rendering_info.pColorAttachments    = color_attachments[0];
	rendering_info.pDepthAttachment     = depth_attachment;
	rendering_info.pStencilAttachment   = nullptr;
	rendering_info.layerCount           = 1;

	return rendering_info;
}

VkRenderingAttachmentInfo vk_color_attachment_info_init(
    VkImageView         view,
    const VkClearValue *clear,
    VkAttachmentLoadOp  load_op,
    VkAttachmentStoreOp store_op,
    VkImageView         resolve_image_view)
{
	VkRenderingAttachmentInfo color_attachment_info = {VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
	color_attachment_info.imageView                 = view;
	color_attachment_info.imageLayout               = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	color_attachment_info.loadOp                    = load_op;
	color_attachment_info.storeOp                   = store_op;

	if (resolve_image_view)
	{
		color_attachment_info.resolveMode        = VK_RESOLVE_MODE_AVERAGE_BIT;
		color_attachment_info.resolveImageView   = resolve_image_view;
		color_attachment_info.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	}
	if (clear)
	{
		color_attachment_info.clearValue = *clear;
	}

	return color_attachment_info;
}

VkRenderingAttachmentInfo vk_depth_attachment_info_init(
    VkImageView         view,
    VkAttachmentLoadOp  load_op,
    VkAttachmentStoreOp store_op)
{
	VkRenderingAttachmentInfo depth_attachment_info     = {VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
	depth_attachment_info.imageView                     = view;
	depth_attachment_info.imageLayout                   = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	depth_attachment_info.loadOp                        = load_op;
	depth_attachment_info.storeOp                       = store_op;
	depth_attachment_info.clearValue.depthStencil.depth = 0.0f;

	return depth_attachment_info;
}

static void
    allocator_init()
{
	VmaAllocatorCreateInfo allocator_create_info{};
	allocator_create_info.device           = vk_renderer.vk_ctx.logical_device;
	allocator_create_info.physicalDevice   = vk_renderer.vk_ctx.physical_device;
	allocator_create_info.vulkanApiVersion = VK_API_VERSION_1_4;
	allocator_create_info.instance         = vk_renderer.vk_ctx.instance;
	allocator_create_info.flags =
	    VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

	vk_check(
	    vmaCreateAllocator(&allocator_create_info, &vk_renderer.allocator));
}

static void
    allocator_fini()
{
	vmaDestroyAllocator(vk_renderer.allocator);
}

static void
    render_images_init()
{
	uint32_t color_image_flag = 0;
	color_image_flag |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	color_image_flag |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	color_image_flag |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	color_image_flag |= VK_IMAGE_USAGE_SAMPLED_BIT;

	vk_renderer.color_image = vk_allocated_image_init(
	    vk_renderer.vk_ctx.logical_device,
	    vk_renderer.allocator,
	    color_image_flag,
	    VK_IMAGE_VIEW_TYPE_2D,
	    vk_renderer.image_extent,
	    VK_FORMAT_B8G8R8A8_UNORM,
	    1,
	    1);

	vk_renderer.depth_image = vk_allocated_image_init(
	    vk_renderer.vk_ctx.logical_device,
	    vk_renderer.allocator,
	    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
	    VK_IMAGE_VIEW_TYPE_2D,
	    vk_renderer.image_extent,
	    VK_FORMAT_D32_SFLOAT,
	    1,
	    WenVkRenderer::msaa_samples);

	for (auto &image : vk_renderer.images)
	{
		image = vk_allocated_image_init(
		    vk_renderer.vk_ctx.logical_device,
		    vk_renderer.allocator,
		    VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		    VK_IMAGE_VIEW_TYPE_2D,
		    vk_renderer.image_extent,
		    VK_FORMAT_R16G16_UINT,
		    1,
		    1);
	}
}

static void
    render_images_fini()
{
	vk_allocated_image_fini(
	    &vk_renderer.vk_ctx, vk_renderer.allocator, &vk_renderer.color_image);

	vk_allocated_image_fini(
	    &vk_renderer.vk_ctx, vk_renderer.allocator, &vk_renderer.depth_image);

	for (const auto &image : vk_renderer.images)
	{
		vk_allocated_image_fini(
		    &vk_renderer.vk_ctx, vk_renderer.allocator, &image);
	}
}

static void configure_render_resources()
{
	VkClearValue scene_clear_value     = {.color = {{1.0f, 0.0f, 0.0f, 0.0f}}};
	VkImageView  resolve_image_view    = nullptr;

	// Set up the rendering attachment info.
	vk_renderer.scene_color_attachment = vk_color_attachment_info_init(
	    vk_renderer.color_image.view,
	    &scene_clear_value,
	    VK_ATTACHMENT_LOAD_OP_CLEAR,
	    VK_ATTACHMENT_STORE_OP_STORE,
	    resolve_image_view);

	vk_renderer.scene_depth_attachment = vk_depth_attachment_info_init(
	    vk_renderer.depth_image.view,
	    VK_ATTACHMENT_LOAD_OP_CLEAR,
	    VK_ATTACHMENT_STORE_OP_STORE);

	std::array scene_color_attachments = {&vk_renderer.scene_color_attachment};

	vk_renderer.scene_rendering_info   = vk_rendering_info_init(
        scene_color_attachments,
        &vk_renderer.scene_depth_attachment,
        vk_renderer.image_extent);
}

static void
    frames_init()
{
	for (auto &frame : vk_renderer.frames)
	{
		vk_frame_buffer_init(
		    &vk_renderer.vk_ctx,
		    vk_renderer.vk_ctx.logical_device,
		    vk_renderer.allocator,
		    vk_renderer.vk_ctx.queue_families.graphics,
		    &frame);
	}
}

static void
    frames_fini()
{
	for (auto &frame : vk_renderer.frames)
	{
		vk_frame_buffer_fini(
		    &vk_renderer.vk_ctx, vk_renderer.vk_ctx.logical_device, &frame);
	}
}

static void
    command_buffers_init()
{
	vk_command_buffer_init(
	    &vk_renderer.vk_ctx,
	    vk_renderer.vk_ctx.logical_device,
	    vk_renderer.vk_ctx.queue_families.graphics,
	    VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
	    &vk_renderer.immediate_cmd_ctx);

	vk_command_buffer_init(
	    &vk_renderer.vk_ctx,
	    vk_renderer.vk_ctx.logical_device,
	    vk_renderer.vk_ctx.queue_families.compute,
	    VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
	    &vk_renderer.compute_cmd_ctx);
}

void command_buffers_fini()
{
	vk_command_buffer_fini(
	    &vk_renderer.vk_ctx,
	    vk_renderer.vk_ctx.logical_device,
	    &vk_renderer.immediate_cmd_ctx);

	vk_command_buffer_fini(
	    &vk_renderer.vk_ctx,
	    vk_renderer.vk_ctx.logical_device,
	    &vk_renderer.compute_cmd_ctx);
}

void resize()
{
	vkDeviceWaitIdle(vk_renderer.vk_ctx.logical_device);

	vk_swapchain_ctx_reset(
	    &vk_renderer.swapchain_context,
	    &vk_renderer.vk_ctx);
	render_images_fini();
	render_images_init();

	for (auto &frame : vk_renderer.frames)
	{
		vk_frame_buffer_reset_sync(
		    &vk_renderer.vk_ctx,
		    vk_renderer.vk_ctx.logical_device,
		    &frame);
	}
}

void update_current_frame_index()
{
	vk_renderer.current_frame_index =
	    vk_renderer.frame_number % vk_renderer.frames.size();
}

void set_renderer_state(VkCommandBuffer command_buffer)
{
	VkViewport viewport{};
	viewport.x        = 0.0f;
	viewport.y        = WEN_CAST(float, vk_renderer.image_extent.height);
	viewport.width    = WEN_CAST(float, vk_renderer.image_extent.width);
	viewport.height   = WEN_CAST(float, vk_renderer.image_extent.height) * -1;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkExtent2D scissor_extent{};
	scissor_extent.width  = WEN_CAST(uint32_t, vk_renderer.render_area.scissor_dimension.x);
	scissor_extent.height = WEN_CAST(uint32_t, vk_renderer.render_area.scissor_dimension.y);

	VkOffset2D scissor_offset{};
	scissor_offset.x = 0;
	scissor_offset.y = 0;

	VkRect2D scissor{};
	scissor.extent = scissor_extent;
	scissor.offset = scissor_offset;

	vkCmdSetViewport(command_buffer, 0, 1, &viewport);

	vkCmdSetScissor(command_buffer, 0, 1, &scissor);

	vkCmdSetCullMode(command_buffer, VK_CULL_MODE_BACK_BIT);

	vkCmdSetFrontFace(command_buffer, VK_FRONT_FACE_COUNTER_CLOCKWISE);

	vkCmdSetPrimitiveTopology(command_buffer, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
}