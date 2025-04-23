#pragma once

#include <VkBootstrap.h>
#include <glm/mat4x4.hpp>

#include "wvk/common/wvk_defines.hpp"
#include "wvk/common/wvk_pch.hpp"
#include "wvk/core/wvk_core.hpp"

#include "wvk_gfxpipeline.hpp"
#include "wvk_gfxtypes.hpp"
#include "wvk_gfxvulkan.hpp"

struct GLFWwindow;

namespace wvk::gfx
{

struct Glyph
{
	glm::vec2  uv0;                ///< top left
	glm::vec2  uv1;                ///< bottom right
	glm::ivec2 bearing;            ///< top left corner, relative to origin on the baseline
	int        advance = 0;        ///< offset to the next char in pixels
};
struct Font
{
	bool load(gfx::Instance &instance, const std::filesystem::path &path, int size, bool antialiasing = true);
	bool load(gfx::Instance &instance, const std::filesystem::path &path, int size, const std::unordered_set<uint32_t> &neededCodePoints, bool antialiasing = true);

	glm::vec2 get_glyph_atlas_size() const;
	glm::vec2 get_glyph_size(uint32_t codePoint) const;

	void for_each_glyph();

	core::Rectangle calc_text_bounding_box(std::string_view text) const;

	std::unordered_map<uint32_t, Glyph> glyphs;
	std::unordered_set<uint32_t>        loadedCodePoints;

	TextureId glyphAtlasId = NULL_TEXTURE_ID;
	glm::vec2 atlasSize;

	int   size        = 0;
	float lineSpacing = 0;
	float ascenderPX  = 0;
	float descenderPX = 0;

	bool is_loaded = false;
};

// ---------------------------------------------------------------------------------------------
// gfx::Instance
// ---------------------------------------------------------------------------------------------
#pragma region gfx::Instance
class WVK_API  Instance final
{
  public:
	struct FrameData
	{
		VkCommandPool   primaryPool          = VK_NULL_HANDLE;
		VkCommandBuffer primaryCommandBuffer = VK_NULL_HANDLE;
	};

  public:
	Instance();
	~Instance();

	Instance(const Instance &)            = delete;
	Instance &operator=(const Instance &) = delete;
	Instance(Instance &&) noexcept;
	Instance &operator=(wvk::gfx::Instance &&) noexcept;

	void init(GLFWwindow *window, const char *appName, bool vsync);
	void cleanup();

	TextureId get_white_texture_id();
	TextureId get_error_texture_id();

	[[nodiscard]] std::shared_ptr<Buffer> create_buffer(VkDeviceSize size, VkBufferUsageFlags usages, VmaMemoryUsage memoryUsages, std::string_view name = "") const;
	[[nodiscard]] std::shared_ptr<Buffer> create_persistent_buffer(VkDeviceSize size, VkBufferUsageFlags usages, std::string_view name = "") const;
	[[nodiscard]] std::shared_ptr<Buffer> create_staging_buffer(VkDeviceSize size, VkBufferUsageFlags usages, Buffer *actualBuffer, std::string_view name = "") const;
	[[nodiscard]] std::shared_ptr<Buffer> create_staging_buffer(VkDeviceSize size, VkBufferUsageFlags usages, std::string_view name = "") const;
	[[nodiscard]] VkDeviceAddress         get_buffer_address(const std::shared_ptr<Buffer> &buffer) const;
	static void                           destroy_buffer(std::shared_ptr<Buffer> &buffer);
	void                                  upload_buffer_to_gpu(VkCommandBuffer cmd, Buffer *gpuBuffer, const void *data, VkDeviceSize totalSize, uint32_t offset) const;

	VkCommandBuffer begin_frame();
	void            end_frame(VkCommandBuffer cmd, const std::shared_ptr<Texture> &drawImage, const EndFrameParams &params);

	inline void wait_idle() const;

	[[nodiscard]] VkDevice     get_device() const;
	[[nodiscard]] VmaAllocator get_allocator() const;

	TextureId create_texture(const CreateImageInfo &createInfo, std::string_view debugName = "", void *pixelData = nullptr, TextureId = NULL_TEXTURE_ID);

	[[nodiscard]] std::shared_ptr<Texture> create_draw_image(VkFormat format, glm::ivec2 size, std::string_view name = "") const;

	[[nodiscard]] inline VkExtent2D swapchain_extent() const;
	[[nodiscard]] bool              required_swapchain_reinit() const;
	void                            recreate_swapchain(uint32_t width, uint32_t height);

	[[nodiscard]] VkDescriptorSetLayout  get_bindless_desc_set_layout() const;
	[[nodiscard]] const VkDescriptorSet &get_bindless_desc_set() const;
	void                                 bind_bindless_desc_set(VkCommandBuffer cmd, VkPipelineLayout layout) const;

	[[nodiscard]] uint32_t get_current_frame_index() const;
	FrameData             &get_current_frame();

  private:
	struct InstanceImpl;
	std::unique_ptr<InstanceImpl> _instance_impl;
};
#pragma endregion

// ---------------------------------------------------------------------------------------------
// gfx::Sprite
// ---------------------------------------------------------------------------------------------
#pragma region gfx::Sprite
struct WVK_API Sprite
{
  public:
	Sprite() = default;
	explicit Sprite(const std::shared_ptr<Texture> &texture);

	std::shared_ptr<Texture> texture;
	uint32_t                 texture_id = 0;
	glm::vec2                texture_size;

	glm::vec2 uv0 = glm::vec2{0.f};
	glm::vec2 uv1 = glm::vec2{1.f};

	glm::vec2 pivot = glm::vec2{0.f};
	glm::vec4 color = glm::vec4{1.f, 1.f, 1.f, 1.f};

	void set_texture(const std::shared_ptr<Texture> &texture);
	void set_texture_rect(const core::Rectangle &rec);
	void set_pivot_pixel(const glm::ivec2 &pixel);
};
#pragma endregion

// ---------------------------------------------------------------------------------------------
// gfx::SpriteRenderer
// ---------------------------------------------------------------------------------------------
#pragma region gfx::SpriteRenderer
class WVK_API SpriteRenderer
{
  public:
	static constexpr uint32_t SPRITE_SHADER_ID  = 0;
	static constexpr uint32_t TEXTURE_SHADER_ID = 1;

  public:
	SpriteRenderer() = default;

	void init(Instance &instance, VkFormat drawImageFormat, std::string_view name = "");
	void cleanup(Instance &instance);

	void begin_draw();
	void end_draw();

	void draw(VkCommandBuffer cmd, Instance &instance, const std::shared_ptr<Texture> &drawImage);
	void draw(VkCommandBuffer cmd, Instance &instance, const std::shared_ptr<Texture> &drawImage, const glm::mat4 viewProjection);

	void draw_sprite(Instance &instance, const Sprite &sprite, const glm::vec2 &position, float rotation, const glm::vec2 &scale, uint32_t shaderId = SPRITE_SHADER_ID);

	void draw_sprite(const Sprite &sprite, const glm::mat4 &transform, uint32_t shaderId = SPRITE_SHADER_ID);

	void draw_text();        // TODO

	void draw_rectangle(Instance &instance, int posX, int posY, int width, int height, const glm::vec4 &color);
	void draw_rectangle_v(Instance &instance, const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color);
	void draw_rectangle_rec(Instance &instance, const core::Rectangle &rec, const glm::vec4 &color);
	void draw_rectangle_pro(Instance &instance, const core::Rectangle &rec, const glm::vec2 &origin, float rotation, const glm::vec4 &color);

  private:
	bool _is_initialized = false;

	SpriteRenderingPipeline _drawing_pipeline;

	static const size_t            MAX_SPRITES           = 25000;
	std::vector<SpriteDrawCommand> _sprite_draw_commands = {};
};
#pragma endregion
}        // namespace wvk::gfx
