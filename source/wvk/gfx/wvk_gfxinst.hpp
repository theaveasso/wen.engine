#pragma once

#include <vk_mem_alloc.h>

#include "wvk/common/wvk_defines.hpp"
#include "wvk/gfx/wvk_gfxtypes.hpp"

struct GLFWwindow;

namespace wvk::gfx
{
class Texture;

class Buffer;

class NBuffer;

struct InstanceImpl;

// ---------------------------------------------------------------------------------------------
// gfx::Instance
// ---------------------------------------------------------------------------------------------
#pragma region gfx::Instance

class WVK_API Instance final
{
  public:
	Instance();
	~Instance();

	Instance(const Instance &)            = delete;
	Instance &operator=(const Instance &) = delete;
	Instance(Instance &&) noexcept;
	Instance &operator=(wvk::gfx::Instance &&) noexcept;

	void init(GLFWwindow *window, const char *appName, bool vsync);
	void cleanup();

	shared_ptr<Texture> get_texture(uint32_t index);
	TextureId           get_white_texture_id();
	TextureId           get_error_texture_id();

	[[nodiscard]] Buffer create_buffer(VkDeviceSize size, VkBufferUsageFlags usages, VmaMemoryUsage memoryUsages, std::string_view name = "") const;

	/**
	 * @brief Create buffer that remains mapped for the entire app lifetime and is always CPU-accessible.
	 * This buffer is ideal for uploading data from the CPU to the GPU (e.g., uniform data, dynamic vertices).
	 */
	[[nodiscard]] Buffer create_persistent_buffer(VkDeviceSize size, VkBufferUsageFlags usages, std::string_view name = "") const;

	[[nodiscard]] Buffer create_gpu_buffer(VkDeviceSize size, VkBufferUsageFlags usages, std::string_view name = "") const;

	[[nodiscard]] Buffer create_staging_buffer(VkDeviceSize size, VkBufferUsageFlags usages, Buffer *actualBuffer, std::string_view name = "") const;

	[[nodiscard]] Buffer create_staging_buffer(VkDeviceSize size, VkBufferUsageFlags usages, std::string_view name = "") const;

	[[nodiscard]] NBuffer create_nbuffer(Instance &instance, VkDeviceSize size, VkBufferUsageFlags usages, std::string_view = "") const;

	[[nodiscard]] VkDeviceAddress get_buffer_address(const Buffer &buffer) const;

	static void destroy_buffer(std::shared_ptr<Buffer> &buffer);
	void        upload_buffer_to_gpu(VkCommandBuffer cmd, Buffer *gpuBuffer, const void *data, VkDeviceSize totalSize, uint32_t offset) const;

	VkCommandBuffer begin_frame();
	void            end_frame(VkCommandBuffer cmd, const std::shared_ptr<Texture> &drawImage, const EndFrameParams &params);

	void          imgui_begin_frame();
	void          imgui_end_frame();
	ImGuiContext *get_imgui_ctx();

	inline void wait_idle() const;

	const TracyVkCtx &get_tracy_vk_ctx() const;

	[[nodiscard]] VkDevice     get_device() const;
	[[nodiscard]] VmaAllocator get_allocator() const;

	[[nodiscard]] TextureId create_texture_w_pixels(VkFormat format, VkImageUsageFlags usages, VkExtent2D extent, const void *data, std::string_view name = "");

	[[nodiscard]] std::shared_ptr<Texture> create_draw_image(VkFormat format, glm::ivec2 size, std::string_view name = "") const;

	[[nodiscard]] inline VkExtent2D swapchain_extent() const;
	[[nodiscard]] bool              required_swapchain_reinit() const;
	void                            recreate_swapchain(uint32_t width, uint32_t height);
	VkImageView                     get_swapchain_view(uint32_t frameIndex);

	[[nodiscard]] VkDescriptorSetLayout  get_bindless_desc_set_layout() const;
	[[nodiscard]] const VkDescriptorSet &get_bindless_desc_set() const;
	void                                 bind_bindless_desc_set(VkCommandBuffer cmd, VkPipelineLayout layout) const;

	[[nodiscard]] uint32_t get_current_frame_index() const;
	wvk::FrameData        &get_current_frame();

  private:
	std::unique_ptr<InstanceImpl> _instance_impl;
};
#pragma endregion
}        // namespace wvk::gfx
