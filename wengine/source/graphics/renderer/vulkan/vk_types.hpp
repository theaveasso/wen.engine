#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

struct WenRenderArea
{
  glm::vec2 bottom_left{0};
  glm::vec2 scissor_dimension{0};
};

struct WenVkCommandContext
{
  VkCommandBuffer primary_buffer{};
  VkCommandPool   primary_pool{};
};

struct WenWorldData
{
  glm::mat4 view{1.0f};
  glm::mat4 projection{1.0f};
  glm::vec4 camera_position{};
};

struct WenVkFrameBuffer
{
  WenVkCommandContext command_context{};
  VkSemaphore         render_semaphore{};
  VkSemaphore         present_semaphore{};
  VkFence             render_fence{};
};

struct WenVkAllocatedImage
{
  VkImage       image{};
  VkImageView   view{};
  VkExtent3D    extent3d{};
  VkFormat      format{};
  uint32_t      mip_levels{1};
  VmaAllocation allocation{};
};

struct WenVkDebugger
{
  VkDebugUtilsMessengerEXT         messenger;
  PFN_vkSetDebugUtilsObjectNameEXT obj_name_pfn;
  VkDevice                         logical_device;
};

struct WenDeviceQueues
{
  VkQueue graphics;
  VkQueue present;
  VkQueue transfer;
  VkQueue compute;
};

struct WenDeviceQueueFamilies
{
  uint32_t graphics;
  uint32_t present;
  uint32_t transfer;
  uint32_t compute;
};

struct WenVkContext
{
  VkInstance             instance;
  VkAllocationCallbacks  *allocator;
  VkSurfaceKHR           surface;
  VkPhysicalDevice       physical_device;
  VkDevice               logical_device;
  WenDeviceQueues        queues;
  WenDeviceQueueFamilies queue_families;
  WenVkDebugger          debugger;
};

struct WenVkSwapchainSupportDetail
{
  VkSurfaceCapabilitiesKHR        capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR>   present_modes;
};

struct WenVkSwapchainContext
{
  VkSwapchainKHR              swapchain;
  VkExtent2D                  extent;
  VkFormat                    format;
  VkPresentModeKHR            present_mode;
  std::vector<VkImage>        images;
  std::vector<VkImageView>    image_views;
  WenVkSwapchainSupportDetail support_detail;
};

struct WenVkRenderer
{
  WenVkContext                       vk_ctx{};
  VkExtent2D                         image_extent{};
  WenRenderArea                      render_area;
  VkClearValue                       scene_clear_value{};
  VkRenderingAttachmentInfo          scene_image_attachment{};
  VkRenderingAttachmentInfo          scene_color_attachment{};
  VkRenderingAttachmentInfo          scene_depth_attachment{};
  VkRenderingInfo                    scene_rendering_info{};
  WenVkSwapchainContext              swapchain_context;
  WenVkAllocatedImage                depth_image;
  WenVkAllocatedImage                color_image;
  VmaAllocator                       allocator{};
  VkFence                            immediate_fence{};
  WenVkCommandContext                immediate_cmd_ctx{};
  WenVkCommandContext                compute_cmd_ctx{};
  uint64_t                           frame_number = 1;
  uint8_t                            current_frame_index{};
  std::array<WenVkAllocatedImage, 3> images{};
  std::array<WenVkFrameBuffer, 3>    frames{};
  static constexpr uint32_t          msaa_samples = 1;
};

