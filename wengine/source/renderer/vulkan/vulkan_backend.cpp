#include "wen/renderer/vulkan/vulkan_backend.hpp"

#include "wen/core/wen_logger.hpp"
#include "wen/renderer/vulkan/vulkan_types.inl"

static wen_vulkan_context_t context;

bool vulkan_renderer_backend_initialize(wen_renderer_backend_t* backend_, const char* name_, struct wen_platform_state_t* state_) {
  context.allocator = nullptr;

  VkApplicationInfo app_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
  app_info.apiVersion = VK_API_VERSION_1_3;
  app_info.pApplicationName = name_;
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 1);
  app_info.pEngineName = "Wen Engine";
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);

  VkInstanceCreateInfo create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
  create_info.pApplicationInfo = &app_info;
  create_info.enabledExtensionCount = 0;
  create_info.ppEnabledExtensionNames = nullptr;
  create_info.enabledLayerCount = 0;
  create_info.ppEnabledLayerNames = nullptr;

  VkResult result = vkCreateInstance(&create_info, context.allocator, &context.instance);
  if (result != VK_SUCCESS) {
    WEN_ERROR("vkCreateInstance failed with result : %u", result);
    return false;
  }

  WEN_INFO("Vulkan renderer backend initialized successfully.");
  return true;
}

void vulkan_renderer_backend_shutdown(wen_renderer_backend_t* backend_) {

}

void vulkan_renderer_backend_resize(wen_renderer_backend_t* backend_, uint16_t width_, uint16_t height_) {

}

bool vulkan_renderer_backend_begin_frame(wen_renderer_backend_t* backend_, float deltatime_) {
  return true;
}

bool vulkan_renderer_backend_end_frame(wen_renderer_backend_t* backend_, float deltatime_) {
  return true;
}

