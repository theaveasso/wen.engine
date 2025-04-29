#pragma once

#include <vk_mem_alloc.h>

#include "wvk/common/wvk_defines.hpp"
#include "wvk/gfx/wvk_types.gfx.hpp"

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

	void register_tileset_texture(std::string_view relPath, TextureId id);

	shared_ptr<Texture> get_texture(uint32_t index);

	shared_ptr<Texture> get_texture(std::string_view relPath);

	TextureId get_white_texture_id();

	TextureId get_error_texture_id();

	/// @brief Create buffer that remains mapped for the entire app lifetime and is always CPU-accessible.
	/// This buffer is ideal for uploading data from the CPU to the GPU (e.g., uniform data, dynamic vertices).
	/// @param size Size of the buffer in bytes.
	/// @param usages Buffer usage flags (VkBufferUsageFlags).
	/// @param name Optional debug name.
	/// @return A created Buffer object.
	[[nodiscard]] shared_ptr<Buffer> make_persistent_buffer(
	    VkDeviceSize       size,
	    VkBufferUsageFlags usages = 0,
	    std::string_view   name   = "") const;

	/// @brief Create a GPU buffer that remains mapped for the entire app lifetime and is CPU-accessible.
	/// This buffer is ideal for uploading data from the CPU to the GPU (e.g., uniform buffers, dynamic vertex data).
	/// @param size Size of the buffer in bytes.
	/// @param usages Buffer usage flags (VkBufferUsageFlags).
	/// @param name Optional debug name.
	/// @return A created Buffer object.
	[[nodiscard]] shared_ptr<Buffer> make_gpu_buffer(
	    VkDeviceSize       size,
	    VkBufferUsageFlags usages,
	    std::string_view   name = "") const;

	/// @brief Upload CPU data to an already created GPU buffer.
	/// Assumes the GPU buffer is persistently mapped.
	/// @param gpuBuffer Target buffer to upload data into.
	/// @param data Pointer to the source data.
	/// @param totalSize Size of the data to upload.
	/// @param offset Offset in the buffer where the data should be copied.
	void upload_buffer_to_gpu(
	    const shared_ptr<Buffer> &gpuBuffer,
	    void                     *data,
	    VkDeviceSize              totalSize,
	    VkDeviceSize              offset) const;

	/// @brief Create buffer that remains mapped for the entire app lifetime and is always CPU-accessible.
	/// This buffer is ideal for uploading data from the CPU to the GPU (e.g., uniform data, dynamic vertices).
	/// @param size: Buffer size
	/// @param usages: Buffer usage
	/// @param name: Debug name
	[[nodiscard]] shared_ptr<NBuffer> make_nbuffer(
	    VkDeviceSize       size,
	    VkBufferUsageFlags usages,
	    std::string_view = "") const;

	/// @brief Begin recording commands for the current frame.
	/// @return A new VkCommandBuffer ready for recording.
	VkCommandBuffer begin_frame();

	/// @brief End the current frame and submit the recorded command buffer.
	/// @param cmd Command buffer to end and submit.
	/// @param drawImage The shared pointer image to present.
	/// @param params Additional parameters controlling end-of-frame behavior.
	void end_frame(
	    VkCommandBuffer           &cmd,
	    const shared_ptr<Texture> &drawImage,
	    const EndFrameParams      &params);

	/// @brief Begin a new ImGui frame.
	/// Should be called before ImGui UI logic is issued.
	void imgui_begin_frame();

	/// @brief End the current ImGui frame and record draw commands.
	void imgui_end_frame();

	/// @brief Get the ImGui context used internally by the engine.
	/// @return Pointer to the current ImGuiContext.
	ImGuiContext *get_imgui_ctx();

	/// @brief Wait until the GPU device is idle.
	/// Blocks the CPU until all GPU work is finished.
	inline void wait_idle() const;

	/// @brief Get the Tracy GPU context used for profiling.
	/// @return Reference to the TracyVkCtx.
	[[nodiscard]] const TracyVkCtx &get_tracy_vk_ctx() const;

	/// @brief Get the Vulkan logical device.
	/// @return VkDevice handle.
	[[nodiscard]] VkDevice     get_device() const;

	/// @brief Get the Vulkan Memory Allocator (VMA) instance.
	/// @return VmaAllocator handle.
	[[nodiscard]] VmaAllocator get_allocator() const;

	[[nodiscard]] TextureId make_texture_w_pixels(
	    VkFormat          format,
	    VkImageUsageFlags usages,
	    VkExtent2D        extent,
	    const void       *data,
	    std::string_view  name = "");

	[[nodiscard]] TextureId make_texture_w_path(
	    VkFormat                     format,
	    VkImageUsageFlags            usages,
	    const std::filesystem::path &path,
	    std::string_view             name = "");

	TextureId make_texture(
	    const CreateTextureInfo &createInfo,
	    std::string_view         name = "");

	[[nodiscard]] inline VkExtent2D swapchain_extent() const;
	[[nodiscard]] bool              required_swapchain_reinit() const;
	void                            recreate_swapchain(uint32_t width, uint32_t height);
	VkImageView                     get_swapchain_view(uint32_t frameIndex);

	[[nodiscard]] VkDescriptorSetLayout  get_bindless_desc_set_layout() const;
	[[nodiscard]] const VkDescriptorSet &get_bindless_desc_set() const;
	void                                 bind_bindless_desc_set(VkCommandBuffer cmd, VkPipelineLayout layout) const;

	[[nodiscard]] uint32_t get_current_frame_index() const;
	wvk::FrameData        &get_current_frame();

	VkSampleCountFlagBits get_max_supported_sampling_count();

  private:
	std::unique_ptr<InstanceImpl> _instance_impl;
};
#pragma endregion
}        // namespace wvk::gfx
