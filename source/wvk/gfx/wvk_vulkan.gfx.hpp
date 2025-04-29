#pragma once

#include <VkBootstrap.h>
#include <glm/vec2.hpp>
#include <vk_mem_alloc.h>

#include "wvk/common/wvk_defines.hpp"
#include "wvk/common/wvk_pch.hpp"

#include "wvk_types.gfx.hpp"
#include "wvk_utils.gfx.hpp"

namespace wvk::gfx
{

class Instance;

class Buffer;
class Texture;

class Sampler;

class BindlessSetManager
{
  public:
	explicit BindlessSetManager(
	    VkDevice         device,
	    float            maxAnisotropy,
	    std::string_view name = "");

	void cleanup(
	    VkDevice device);

	[[nodiscard]] VkDescriptorSetLayout get_desc_set_layout() const { return _desc_set_layout; }

	[[nodiscard]] const VkDescriptorSet &get_desc_set() const { return _desc_set; }

	VkDescriptorPool get_desc_pool() { return _desc_pool; }

	void add_image(
	    VkDevice    device,
	    uint32_t    id,
	    VkImageView imageView);

	void add_sampler(
	    VkDevice                        device,
	    uint32_t                        id,
	    const std::shared_ptr<Sampler> &sampler);

  private:
	std::string _debug_name;
	bool        _is_initialized = false;

	VkDescriptorPool      _desc_pool       = VK_NULL_HANDLE;
	VkDescriptorSetLayout _desc_set_layout = VK_NULL_HANDLE;
	VkDescriptorSet       _desc_set        = VK_NULL_HANDLE;

	std::shared_ptr<Sampler> _nearest_sampler    = nullptr;
	std::shared_ptr<Sampler> _linear_sampler     = nullptr;
	std::shared_ptr<Sampler> _shadow_map_sampler = nullptr;
};

class WVK_API Texture final
{
  public:
	WVK_MOVABLE_ONLY(Texture);

	/// @brief Constructs a texture by creating a VkImage and allocating memory.
	/// @param device: Vulkan device handle.
	/// @param allocator: VMA allocator for memory allocation.
	/// @param createInfo: Parameters for creating the texture (format, usage, size, etc.).
	/// @param customAllocationCreateInfo: Optional custom memory allocation settings.
	/// @param name: Optional debug name for the texture resource.
	explicit Texture(
	    VkDevice                               device,
	    VmaAllocator                           allocator,
	    const CreateTextureInfo               &createInfo,
	    std::optional<VmaAllocationCreateInfo> customAllocationCreateInfo,
	    std::string_view                       name = "");

	/// @brief Constructs a texture from an existing VkImage.
	/// @param device: Vulkan device handle.
	/// @param image: Existing VkImage handle.
	/// @param format: Image format.
	/// @param extent: Image dimensions.
	/// @param mipMap: ImageView mip map.
	/// @param numLayers: ImageView base array layer .
	/// @param name: Optional debug name for the texture resource.
	explicit Texture(
	    VkDevice         device,
	    VkImage          image,
	    VkFormat         format,
	    VkExtent3D       extent,
	    uint32_t         mipMap,
	    uint32_t         numLayers,
	    std::string_view name = "");

	/// @brief Destroys the texture and frees associated memory.
	/// @param device: Vulkan device handle.
	/// @param allocator: VMA allocator used for the texture.
	void cleanup(
	    VkDevice     device,
	    VmaAllocator allocator);

	void upload(
	    VkCommandBuffer          &cmd,
	    const shared_ptr<Buffer> &buffer);

	/// @brief Creates a VkImageView for this texture.
	/// @param device: Vulkan device handle.
	/// @param format: Format to use for the view.
	/// @param numMipsLevels: Number of mipmap levels to view.
	/// @param layers: Number of array layers to view.
	/// @return Created VkImageView handle.
	VkImageView make_image_view(
	    VkDevice device,
	    VkFormat format,
	    uint32_t numMipsLevels,
	    uint32_t layers);

	/// @brief Checks if the texture format includes a depth component.
	/// @return True if format is a depth format.
	[[nodiscard]] bool is_depth() const;

	/// @brief Checks if the texture format includes a stencil component.
	/// @return True if format has stencil aspect.
	[[nodiscard]] bool is_stencil() const;

	/// @brief Returns the underlying VkImage handle.
	/// @return Vulkan image handle.
	VkImage handle() { return _handle; }

	/// @brief Returns the format of the texture.
	/// @return Vulkan format enum.
	[[nodiscard]] VkFormat get_format() const { return _format; }

	/// @brief Returns the default VkImageView associated with the texture.
	/// @return Vulkan image view handle.
	VkImageView get_image_view() { return _image_view; }

	/// @brief Returns the memory size (in bytes) allocated for the texture.
	/// @return Size of the allocated memory.
	[[nodiscard]] VkDeviceSize get_size() const { return _size; }

	/// @brief Computes the raw image size (width * height * depth * pixel size).
	/// @return Raw byte size of the image data.
	[[nodiscard]] VkDeviceSize get_image_size() const { return pixel_size_in_bytes() * _extent.width * _extent.height * _extent.depth; }

	/// @brief Returns the number of bytes per pixel based on the texture format.
	/// @return Bytes per pixel.
	[[nodiscard]] uint32_t pixel_size_in_bytes() const { return bytes_per_pixel(_format); }

	/// @brief Returns the 2D size of the texture (width, height).
	/// @return 2D vector containing width and height.
	[[nodiscard]] glm::vec2 get_size_2d() const { return {_extent.width, _extent.height}; }

	/// @brief Returns the dimension (width, height) of the texture.
	/// @return GLM vector 2d of texture .
	const glm::vec2 &get_size2d() { return _size2d; }

	/// @brief Set the dimension (width, height) of the texture.
	void set_size2d(const glm::vec2 &value) { _size2d = value; }

	/// @brief Returns the 2D extent of the texture.
	/// @return Vulkan 2D extent struct.
	[[nodiscard]] VkExtent2D get_extent_2d() const { return VkExtent2D(_extent.width, _extent.height); }

	/// @brief Returns the bindless ID associated with the texture.
	/// @return Bindless ID for shader access.
	[[nodiscard]] uint32_t get_bindless_id() const { return _id; }

	/// @brief Sets the bindless ID for the texture.
	/// @param id New bindless ID value.
	void set_bindless_id(const uint32_t id) { _id = id; }

  private:
	std::string _debug_name;
	bool        _own_image = false;

	TextureId _id = NULL_BINDLESS_ID;

	uint32_t           _mip_levels       = 0;
	uint32_t           _base_array_layer = 0;
	bool               _is_cubemap       = false;
	VkImageUsageFlags  _usages           = 0;
	VkImageAspectFlags _aspect_flags     = 0;

	VkFormat    _format     = VK_FORMAT_B8G8R8A8_SRGB;
	VkImage     _handle     = VK_NULL_HANDLE;
	VkImageView _image_view = VK_NULL_HANDLE;
	VkExtent3D  _extent     = VkExtent3D{1, 1, 1};

	glm::vec2 _size2d = glm::vec2(1);

	VmaAllocation _allocation = {};
	VkDeviceSize  _size       = 0;
};

/// @brief Swapchain wrapper managing swapchain images, synchronization, and presentation.
class Swapchain
{
  public:
	/// @brief Construct a Swapchain.
	/// @param device: Vulkan device handle (from vkb::Device).
	/// @param format Swapchain surface format.
	/// @param width Swapchain width in pixels.
	/// @param height Swapchain height in pixels.
	/// @param vsync Whether to enable vertical synchronization (VSync).
	/// @param name Optional debug name for the swapchain.
	explicit Swapchain(
	    const vkb::Device &device,
	    VkFormat           format,
	    uint32_t           width,
	    uint32_t           height,
	    bool               vsync,
	    std::string_view   name = "");

	/// @brief Clean up swapchain resources.
	/// @param instance Vulkan instance handle wrapper.
	void cleanup(
	    Instance &instance);

	/// @brief Recreate and reinitialize the swapchain, typically after window resize or format change.
	/// @param device Vulkan device handle.
	/// @param format New surface format.
	/// @param width New width.
	/// @param height New height.
	void reinit(
	    const vkb::Device &device,
	    VkFormat           format,
	    uint32_t           width,
	    uint32_t           height);

	/// @brief Begin a new frame by waiting on frame fences.
	/// @param device Vulkan device handle.
	/// @param frameIndex Frame index to wait on.
	void begin_frame(
	    VkDevice device,
	    size_t   frameIndex) const;

	/// @brief Reset the fence associated with a specific frame, preparing it for new GPU work.
	/// @param device Vulkan device handle.
	/// @param frameIndex Frame index whose fence to reset.
	void resent_fence(
	    VkDevice device,
	    size_t   frameIndex) const;

	/// @brief Acquire the next available swapchain image for rendering.
	/// @param device Vulkan device handle.
	/// @param frameIndex Current frame index.
	/// @return A pair containing the image index and a shared pointer to the Texture representing the swapchain image.
	std::pair<uint32_t, shared_ptr<Texture>> acquire_image(
	    VkDevice device,
	    size_t   frameIndex);

	/// @brief Submit the recorded command buffer and present the swapchain image.
	/// @param cmd Command buffer to submit.
	/// @param graphicsQueue Graphics queue handle.
	/// @param frameIndex Current frame index.
	/// @param swapchainImageIndex Index of the swapchain image to present.
	void submit_and_present(
	    VkCommandBuffer &cmd,
	    VkQueue          graphicsQueue,
	    size_t           frameIndex,
	    uint32_t         swapchainImageIndex);

	/// @brief Get the Vulkan image view for a given swapchain image index.
	/// @param swapchainImageIndex Index of the swapchain image.
	/// @return VkImageView handle.
	VkImageView get_image_view(size_t swapchainImageIndex) { return _swapchain_images[swapchainImageIndex]->get_image_view(); }

	/// @brief Get the total number of swapchain images.
	/// @return Number of swapchain images.
	[[nodiscard]] uint32_t get_image_count() const { return _swapchain.image_count; }

	/// @brief Check if the swapchain needs to be recreated (e.g., after window resize or surface lost).
	/// @return True if recreation is needed, false otherwise.
	[[nodiscard]] bool required_reinit() const { return _dirty; }

	/// @brief Get the index of the currently acquired swapchain image.
	/// @return Current swapchain image index.
	[[nodiscard]] uint32_t current_swapchain_image_index() const { return _current_image_index; }

	/// @brief Get the current extent (resolution) of the swapchain.
	/// @return Swapchain extent (width and height).
	[[nodiscard]] VkExtent2D extent() const { return _swapchain.extent; }

  private:
	struct FrameData
	{
		VkSemaphore swapchain_semaphore;
		VkSemaphore render_semaphore;
		VkFence     render_fence;
	};

	bool           _is_initialized = false;
	std::string    _debug_name;
	vkb::Swapchain _swapchain = {};

	bool _dirty = false;
	bool _vsync = true;

	std::array<FrameData, FRAME_OVERLAP> _frames{};

	std::vector<shared_ptr<Texture>> _swapchain_images;
	uint32_t                         _current_image_index = 0;
};

class Shader
{
  public:
	WVK_MOVABLE_ONLY(Shader)
	explicit Shader(
	    VkDevice              device,
	    VkShaderStageFlagBits stage,
	    std::string_view      filename,
	    std::string_view      entryPoint,
	    std::string_view      name);

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

class Buffer final
{
  public:
	WVK_MOVABLE_ONLY(Buffer)
	explicit Buffer(
	    VkDevice                  device,
	    VmaAllocator              vmaAllocator,
	    VmaAllocationCreateInfo  *allocationInfo,
	    const VkBufferCreateInfo *bufferInfo,
	    std::string_view          name = "");

	void cleanup();

	VkBuffer handle() const { return _handle; }

	VkDeviceSize size() const { return _size; }

	void upload(
	    VkCommandBuffer           cmd,
	    const shared_ptr<Buffer> &gpuBuffer,
	    void                     *data,
	    VkDeviceSize              size,
	    VkDeviceSize              offset = 0,
	    bool                      sync   = false) const;

	void copy_data_to_buffer(
	    const void  *data,
	    VkDeviceSize size,
	    VkDeviceSize offset = 0) const;

	void *get_mapped_data() { return _mapped_memory; }

	VkDeviceAddress get_device_address(VkDevice device) const;

  private:
	bool        _is_initialized = false;
	std::string _debug_name;

	VmaAllocator             _allocator;
	VmaAllocation            _allocation             = VK_NULL_HANDLE;
	VmaAllocationInfo        _allocation_info        = {};
	VmaAllocationCreateInfo *_allocation_create_info = {};

	VkBuffer                _handle                = VK_NULL_HANDLE;
	VkDeviceSize            _size                  = 0;
	mutable void           *_mapped_memory         = nullptr;
	mutable VkDeviceAddress _buffer_device_address = 0;
};

class NBuffer final
{
  public:
	NBuffer() = default;
	WVK_MOVABLE_ONLY(NBuffer);

	explicit NBuffer(
	    const Instance    &instance,
	    VkDeviceSize       size,
	    VkBufferUsageFlags usages,
	    size_t             numFramesInFlight,
	    std::string_view   name = "nbuffer");

	void cleanup();

	void upload(
	    VkCommandBuffer cmd,
	    size_t          frameIndex,
	    void           *data,
	    VkDeviceSize    size,
	    VkDeviceSize    offset,
	    bool            sync = true);

	const shared_ptr<Buffer> &handle() { return _gpu_buffer; }

	[[nodiscard]] size_t size() const { return _gpu_buffer_size; }

  private:
	bool        _is_initialized = false;
	std::string _debug_name;

	size_t                          _frames_in_flight = 0;
	size_t                          _gpu_buffer_size  = 0;
	std::vector<shared_ptr<Buffer>> _stagings;
	shared_ptr<Buffer>              _gpu_buffer;
};

class Sampler
{
  public:
	WVK_MOVABLE_ONLY(Sampler);

	explicit Sampler(VkDevice device, const VkSamplerCreateInfo *createInfo, std::string_view name = "");

	void cleanup(VkDevice device);

	[[nodiscard]] VkSampler handle() const;

  private:
	std::string _debug_name;
	bool        _is_initialized = false;

	VkSampler _handle = VK_NULL_HANDLE;
};

class ImmediateCommandQueue
{
  public:
	explicit ImmediateCommandQueue(
	    VkDevice         device,
	    uint32_t         graphicsFamilyIndex,
	    VkQueue          graphicsQueue,
	    std::string_view name = "");

	void cleanup(
	    Instance &instance);

	void submit(
	    VkDevice                                   device,
	    std::function<void(VkCommandBuffer cmd)> &&callback) const;

  private:
	std::string _debug_name;
	bool        _is_initialized = false;

	VkQueue _graphics_queue = VK_NULL_HANDLE;

	VkCommandPool   _command_pool   = VK_NULL_HANDLE;
	VkCommandBuffer _command_buffer = VK_NULL_HANDLE;
	VkFence         _fence          = VK_NULL_HANDLE;
};
}        // namespace wvk::gfx
