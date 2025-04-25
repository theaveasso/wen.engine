#pragma once

#include <VkBootstrap.h>
#include <vk_mem_alloc.h>

#include "wvk/common/wvk_defines.hpp"
#include "wvk/common/wvk_pch.hpp"
#include "wvk_gfxutils.hpp"

namespace wvk::gfx
{
class Instance;

class Buffer;

class Texture;

class Sampler;

using BufferVec  = std::vector<std::shared_ptr<Buffer>>;
using TextureVec = std::vector<std::shared_ptr<Texture>>;
using SamplerVec = std::vector<std::shared_ptr<Sampler>>;

#pragma region vk::BindlessSetManager

class BindlessSetManager
{
  public:
	BindlessSetManager() = default;

	explicit BindlessSetManager(VkDevice device, float maxAnisotropy, std::string_view name = "");

	void cleanup(VkDevice device);

	[[nodiscard]] VkDescriptorSetLayout get_desc_set_layout() const { return _desc_set_layout; }

	[[nodiscard]] const VkDescriptorSet &get_desc_set() const { return _desc_set; }

	VkDescriptorPool get_desc_pool() { return _desc_pool; }

	void add_image(VkDevice device, uint32_t id, VkImageView imageView);

	void add_sampler(VkDevice device, uint32_t id, const std::shared_ptr<Sampler> &sampler);

  private:
	std::string _debug_name;

	VkDescriptorPool      _desc_pool       = VK_NULL_HANDLE;
	VkDescriptorSetLayout _desc_set_layout = VK_NULL_HANDLE;
	VkDescriptorSet       _desc_set        = VK_NULL_HANDLE;

	std::shared_ptr<Sampler> _nearest_sampler    = nullptr;
	std::shared_ptr<Sampler> _linear_sampler     = nullptr;
	std::shared_ptr<Sampler> _shadow_map_sampler = nullptr;

  private:
	void init(VkDevice device, float maxAnisotropy);

	void init_default_sampler(VkDevice device, float maxAnisotropy);
};

#pragma endregion

#pragma region vk::Swapchain

class Swapchain
{
  public:
	Swapchain() = default;

	Swapchain(const vkb::Device &device, VkFormat format, uint32_t width, uint32_t height, bool vsync,
	          std::string_view name = "");

	void reinit(const vkb::Device &device, VkFormat format, uint32_t width, uint32_t height);

	void cleanup(VkDevice device);

	void begin_frame(VkDevice device, size_t frameIndex) const;

	void resent_fence(VkDevice device, size_t frameIndex) const;

	std::pair<uint32_t, std::shared_ptr<Texture>> acquire_image(VkDevice device, size_t frameIndex);

	void submit_and_present(VkCommandBuffer cmd, VkQueue graphicsQueue, size_t frameIndex, uint32_t swapchainImageIndex);

	VkImageView get_image_view(size_t swapchainImageIndex);
	uint32_t    get_image_count() { return _swapchain.image_count; }

	[[nodiscard]] bool required_reinit() const;

	[[nodiscard]] uint32_t current_swapchain_image_index() const;

	[[nodiscard]] VkExtent2D swapchain_extent() const;

  private:
	bool        _is_initialized = false;
	std::string _debug_name;

	bool _dirty = false;
	bool _vsync = true;

	struct FrameData
	{
		VkSemaphore swapchain_semaphore;
		VkSemaphore render_semaphore;
		VkFence     render_fence;
	};

	std::array<FrameData, FRAME_OVERLAP> _frames{};

	vkb::Swapchain _swapchain = {};

	TextureVec _swapchain_images;
	uint32_t   _current_image_index = 0;

  private:
	void init(const vkb::Device &device, VkFormat format, uint32_t width, uint32_t height);
};

#pragma endregion

#pragma region vk::Shader

class Shader
{
  public:
	WVK_MOVABLE_ONLY(Shader)

	explicit Shader(VkDevice device, VkShaderStageFlagBits stage, std::string_view filename,
	                std::string_view entryPoint, std::string_view name);

	void cleanup(VkDevice device);

	[[nodiscard]] VkShaderModule handle() const;

	[[nodiscard]] VkShaderStageFlagBits stage() const;

  private:
	std::string _debug_name;

	VkShaderModule        _module = VK_NULL_HANDLE;
	VkShaderStageFlagBits _stage  = VK_SHADER_STAGE_ALL;
	std::string           _entry_point;

  private:
	void init(VkDevice device, std::string_view filename);
};

#pragma endregion

/**
 * @brief Vulkan GPU Buffer abstraction that manages both staging and device-local buffers.
 * 	      Supports direct uploads, staging uploads, handle views, and device address queries.
 */
#pragma region vk::Buffer

class Buffer final
{
  public:
	Buffer() = default;
	explicit Buffer(VkDevice device, VmaAllocator vmaAllocator, VmaAllocationCreateInfo *allocationInfo, const VkBufferCreateInfo *bufferInfo, std::string_view name = "");

	void cleanup();

	inline VkBuffer     handle() const { return _handle; }
	inline VkDeviceSize size() const { return _size; }

	void upload(VkCommandBuffer cmd, Buffer *gpuBuffer, void *data, VkDeviceSize size, VkDeviceSize offset, bool sync = false) const;

	void copy_data_to_buffer(const void *data, VkDeviceSize size, VkDeviceSize offset = 0) const;

	void *get_mapped_data() { return _mapped_memory; }

	VkDeviceAddress device_address(VkDevice device) const;

  private:
	std::string _debug_name;

	VmaAllocator             _allocator;
	VmaAllocation            _allocation             = VK_NULL_HANDLE;
	VmaAllocationInfo        _allocation_info        = {};
	VmaAllocationCreateInfo *_allocation_create_info = {};
	mutable VkDeviceAddress  _buffer_device_address  = 0;
	mutable void            *_mapped_memory          = nullptr;

	VkDeviceSize                               _size;
	VkBuffer                                   _handle                   = VK_NULL_HANDLE;
	VkBufferUsageFlags                         _usages                   = 0;
	Buffer                                    *_actual_buffer_if_staging = nullptr;
	std::unordered_map<VkFormat, VkBufferView> _buffer_views;
};

/**
 * NBuffer
 */
class NBuffer final
{
  public:
	NBuffer() = default;

	explicit NBuffer(Instance &instance, VkDeviceSize size, VkBufferUsageFlags usages, size_t numFramesInFlight, std::string_view name = "");

	void init(Instance &instance, VkDeviceSize size, VkBufferUsageFlags usages, size_t numFramesInFlight, std::string_view name = "");
	void cleanup(VkDevice device);

	void upload(VkCommandBuffer cmd, size_t frameIndex, void *data, VkDeviceSize size, VkDeviceSize offset, bool sync = true);

	Buffer handle() { return _gpu_buffer; }

	size_t size() { return _gpu_buffer_size; }

  private:
	bool        _is_initialized = false;
	std::string _debug_name;

	size_t              _frames_in_flight = 0;
	size_t              _gpu_buffer_size  = 0;
	std::vector<Buffer> _stagings;
	Buffer              _gpu_buffer;
};
#pragma endregion

/**
 * @brief
 */
#pragma region vk::Texture

class WVK_API Texture final
{
  public:
	Texture() = default;

	static const auto NULL_BINDLESS_ID = std::numeric_limits<uint32_t>::max();

	Texture(VkDevice device, VkImage image, VkFormat format, VkExtent3D extents, uint32_t numLayers, bool multiView, std::string_view name = "");

	Texture(VkDevice device, VmaAllocator allocator, const CreateTextureInfo &createInfo, std::optional<VmaAllocationCreateInfo> customAllocationCreateInfo, std::string_view name = "");

	void cleanup(VkDevice device);

	[[nodiscard]] VkImage handle() const { return _handle; }

	[[nodiscard]] VkFormat format() const { return _format; }

	[[nodiscard]] VkImageAspectFlags aspect_flag() const;

	[[nodiscard]] VkDeviceSize image_size() const { return _size; }

	[[nodiscard]] VkDeviceSize size() const { return pixel_size_in_bytes() * extent3d().width * extent3d().height * extent3d().depth; }

	[[nodiscard]] VkExtent2D extent() const { return VkExtent2D{.width = _extents.width, .height = _extents.height}; }

	[[nodiscard]] VkExtent3D extent3d() const { return _extents; }

	[[nodiscard]] glm::vec2 get_size_2d() const { return _size_2d; }

	glm::vec2 get_mutable_size_2d() { return _size_2d; }

	void set_size_2d(const glm::vec2 &size2d) { _size_2d = size2d; }

	[[nodiscard]] bool is_depth() const;

	[[nodiscard]] bool is_stencil() const;

	//	void update_and_generate_mips(VkCommandBuffer cmd, const Buffer *stagingBuffer, void *data);
	void upload_only(VkCommandBuffer cmd, const Buffer *stagingBuffer, uint32_t layer = 0);

	VkImageView image_view(uint32_t mipLevel = UINT32_MAX);

	VkImageView create_image_view(VkDevice device, VkFormat format, uint32_t numMipsLevels, uint32_t layers);

	void set_bindless_id(const uint32_t id) { _id = id; }

	[[nodiscard]] inline uint32_t get_bindless_id() const { return _id; }

	[[nodiscard]] inline uint32_t pixel_size_in_bytes() const { return bytes_per_pixel(_format); }

	[[nodiscard]] bool is_initialized() const { return _id != NULL_BINDLESS_ID; }

  private:
	std::string _debug_name;

	VmaAllocator            _allocator              = VK_NULL_HANDLE;
	VmaAllocationCreateInfo _allocation_create_info = {};
	VmaAllocation           _allocation             = VK_NULL_HANDLE;
	VmaAllocationInfo       _allocation_info        = {};

	VkDeviceSize                              _size   = 0;
	glm::vec2                                 _size_2d{};
	VkImageCreateFlags                        _flags  = 0;
	VkImageUsageFlags                         _usage  = 0;
	VkImageType                               _type   = VK_IMAGE_TYPE_2D;
	VkImageLayout                             _layout = VK_IMAGE_LAYOUT_UNDEFINED;
	VkImage                                   _handle = VK_NULL_HANDLE;
	VkImageView                               _view   = VK_NULL_HANDLE;
	VkFormat                                  _format = VK_FORMAT_UNDEFINED;
	VkExtent3D                                _extents;
	VkImageViewType                           _view_type     = VK_IMAGE_VIEW_TYPE_2D;
	VkSampleCountFlags                        _msaa_samples  = VK_SAMPLE_COUNT_1_BIT;
	VkImageTiling                             _tiling        = VK_IMAGE_TILING_OPTIMAL;
	bool                                      _own_image     = false;
	uint32_t                                  _mip_levels    = 1;
	uint32_t                                  _layer_count   = 1;
	bool                                      _multi_view    = false;
	bool                                      _generate_mips = false;
	std::unordered_map<uint32_t, VkImageView> _image_view_framebuffers;

	uint32_t _id = NULL_BINDLESS_ID;
};

#pragma endregion

/**
 * @brief Wrapper class for creating and managing a Vulkan handle object.
 *
 * A Vulkan handle defines how to sample textures in shaders—this includes filtering (minification and magnification),
 * addressing modes (what to do when UV coordinates go beyond [0, 1]), mipmapping behavior, and optionally depth comparisons.
 */
#pragma region vk::Sampler

class Sampler
{
  public:
	WVK_MOVABLE_ONLY(Sampler);

	explicit Sampler(VkDevice device, const VkSamplerCreateInfo *createInfo, std::string_view name = "");

	void cleanup(VkDevice device);

	[[nodiscard]] VkSampler handle() const;

  private:
	std::string _debug_name;

	VkSampler _handle = VK_NULL_HANDLE;
};

#pragma endregion
#pragma region vk::ImmediateCommandQueue

class ImmediateCommandQueue
{
  public:
	ImmediateCommandQueue() = default;

	explicit ImmediateCommandQueue(VkDevice device, uint32_t graphicsFamilyIndex, VkQueue graphicsQueue,
	                               std::string_view name = "immediate command");

	void cleanup(VkDevice device);

	void submit(VkDevice device,

	            std::function<
	                void(VkCommandBuffer
	                         cmd)> &&callback) const;

  private:
	std::string _debug_name;

	bool _is_initialized = false;

	VkQueue _graphics_queue = VK_NULL_HANDLE;

	VkCommandPool   _command_pool   = VK_NULL_HANDLE;
	VkCommandBuffer _command_buffer = VK_NULL_HANDLE;
	VkFence         _fence          = VK_NULL_HANDLE;

  private:
	void init(VkDevice device, uint32_t graphicsFamilyIndex);
};

#pragma endregion
}        // namespace wvk::gfx