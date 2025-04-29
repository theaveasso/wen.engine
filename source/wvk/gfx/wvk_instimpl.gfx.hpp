#pragma once

// clang-format off
#include <vulkan/vulkan.h>
#include <volk.h>
#include <VkBootstrap.h>
#include <vk_mem_alloc.h>
#include <tracy/TracyVulkan.hpp>
// clang-format on

#include "wvk/common/wvk_defines.hpp"
#include "wvk_types.gfx.hpp"

struct GLFWwindow;

namespace wvk::gfx
{
class Swapchain;
class ImmediateCommandQueue;
class BindlessSetManager;
class Buffer;
class NBuffer;
class Texture;

class UIOverlay;
class Instance;

struct WVK_API InstanceImpl
{
	std::string appName;

	vkb::Instance          instance       = {};
	vkb::PhysicalDevice    physicalDevice = {};
	vkb::Device            device         = {};
	VmaAllocator           allocator      = VK_NULL_HANDLE;
	VkFormat               format         = VK_FORMAT_B8G8R8A8_SRGB;
	VkSurfaceKHR           surface        = VK_NULL_HANDLE;
	unique_ptr<Swapchain>             swapchain;
	unique_ptr<ImmediateCommandQueue> immediateCommandQueue;
	unique_ptr<BindlessSetManager>    bindlessSetManager;

	shared_ptr<UIOverlay> imguiRenderer{};

	std::array<wvk::FrameData, FRAME_OVERLAP> frames;
	uint32_t                                  frameNumber = 0;

	VkQueue  graphicsQueue            = VK_NULL_HANDLE;
	uint32_t graphicsFamilyQueueIndex = 0;

	VkSampleCountFlagBits supportedSampleCounts;
	VkSampleCountFlagBits highestSupportedSamples = VK_SAMPLE_COUNT_1_BIT;
	float                 maxSamplerAnisotropy    = 1.0f;

	bool vsync         = false;
	bool isInitialized = false;

	void _vulkan_init(GLFWwindow *window);

	void _create_vma_allocator();
	void _create_frame_data();

	void _check_device_capabilities();

	void _create_default_texture();

	TextureId _create_texture_w_pixels(VkFormat textureFormat, VkImageUsageFlags usages, VkExtent2D extent, const void *data, std::string_view = "");

	[[nodiscard]] shared_ptr<Buffer> _make_staging_buffer(
	    VkDeviceSize       size,
	    VkBufferUsageFlags usages,
	    std::string_view   name = "") const;

	VkCommandBuffer _begin_frame();
	void            _end_frame();

	// textures
	TextureId whiteTextureId = NULL_TEXTURE_ID;
	TextureId errorTextureId = NULL_TEXTURE_ID;

	std::unordered_map<std::string, wvk::TextureId> tilesetPathToTextureId;
	std::vector<std::shared_ptr<Texture>> textures = {};
	[[nodiscard]] inline TextureId        _get_free_texture_id() const { return WVK_CAST(uint32_t, textures.size()); }
	TextureId                             _add_texture(TextureId id, const std::shared_ptr<Texture> &texture);

	void _destroy_textures();

	FrameData             &_get_current_frame_data() { return frames[_get_current_frame_index()]; }
	[[nodiscard]] uint32_t _get_current_frame_index() const { return frameNumber % FRAME_OVERLAP; }
};

}        // namespace wvk::gfx
