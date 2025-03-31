#include "wen/renderer/vulkan/vulkan_backend.hpp"

#include "wen/core/wen_logger.hpp"
#include "wen/renderer/vulkan/vulkan_platform.hpp"
#include "wen/renderer/vulkan/vulkan_types.inl"

#include "wen/datastructures/wen_strbuf.hpp"
#include "wen/datastructures/wen_vec.hpp"
#include "wen/platform/wen_platform.hpp"

static wen_vulkan_context_t context;

VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data);

bool vulkan_renderer_backend_initialize(wen_renderer_backend_t* backend_, const char* name_, struct wen_platform_state_t* state_) {
  context.allocator = nullptr;

  VkApplicationInfo app_info  = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
  app_info.apiVersion         = VK_API_VERSION_1_3;
  app_info.pApplicationName   = name_;
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 1);
  app_info.pEngineName        = "Wen Engine";
  app_info.engineVersion      = VK_MAKE_VERSION(1, 0, 0);

  VkInstanceCreateInfo create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
  create_info.pApplicationInfo     = &app_info;

  /** Obtain a list of required extensions. */
  const char** required_exts = vec_create(const char*);
  vec_push(required_exts, &VK_KHR_SURFACE_EXTENSION_NAME);
  platform_get_required_exts_names(&required_exts);
#if defined(WEN_DEBUG)
  vec_push(required_exts, &VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  wen_debug("[Vulkan Backend] - required extensions");
  for (int i = 0; i < vec_len(required_exts); ++i) {
    wen_debug(required_exts[i]);
  }
#endif
  create_info.enabledExtensionCount   = vec_len(required_exts);
  create_info.ppEnabledExtensionNames = required_exts;

  const char** required_validation_layer_names = nullptr;
  uint32_t     required_validation_layer_count = 0;
#if defined(WEN_DEBUG)
  wen_debug("[Vulkan Backend] - Validation layers enabled.");
  required_validation_layer_names = vec_create(const char*);
  vec_push(required_validation_layer_names, &"VK_LAYER_KHRONOS_validation");
  required_validation_layer_count = vec_len(required_validation_layer_names);

  // Obtain a list of available validation layers
  uint32_t available_layer_count = 0;
  vk_check(vkEnumerateInstanceLayerProperties(&available_layer_count, nullptr));
  auto* available_layers = (VkLayerProperties*)vec_reserve(VkLayerProperties, available_layer_count);
  vk_check(vkEnumerateInstanceLayerProperties(&available_layer_count, available_layers));

  for (uint32_t i = 0; i < required_validation_layer_count; ++i) {
    wen_info("[Vulkan Backend] - Searching for layer %s ...", required_validation_layer_names[i]);
    bool found = false;
    for (uint32_t j = 0; j < available_layer_count; ++j) {
      if (wen_str_eq(required_validation_layer_names[i], available_layers[j].layerName)) {
        found = true;
        wen_info("[Vulkan Backend] - Searching for layer %s found!", required_validation_layer_names[i]);
        break;
      }
    }
    if (!found) {
      wen_info("[Vulkan Backend] - Required validation layer %s missing!", required_validation_layer_names[i]);
      return false;
    }
  }
  wen_info("[Vulkan Backend] - All required validation layers are present.");
#endif

  create_info.enabledLayerCount   = required_validation_layer_count;
  create_info.ppEnabledLayerNames = required_validation_layer_names;

  vk_check(vkCreateInstance(&create_info, context.allocator, &context.instance));

  wen_info("[Vulkan Backend] - Vulkan instance created.");
  wen_info("[Vulkan Backend] - renderer backend initialized successfully.");

#if defined(WEN_DEBUG)
  wen_debug("[Vulkan Backend] - Creating Vulkan debugger.");
  uint32_t log_severity = 0;
  log_severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  log_severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
  //  log_severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
  //  log_severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;

  uint32_t message_type = 0;
  message_type |= VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
  message_type |= VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
  message_type |= VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

  VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
  debug_create_info.messageSeverity                    = log_severity;
  debug_create_info.messageType                        = message_type;
  debug_create_info.pfnUserCallback                    = vk_debug_callback;

  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context.instance, "vkCreateDebugUtilsMessengerEXT");
  wen_assert(func, "failed to create debug messenger!");
  vk_check(func(context.instance, &debug_create_info, context.allocator, &context.debug_messenger));
#endif
  return true;
}

void vulkan_renderer_backend_shutdown(wen_renderer_backend_t* backend_) {
#if defined(WEN_DEBUG)
  wen_debug("[Vulkan Backend] - Destroying Vulkan debugger.");
  if (context.debug_messenger) {
    auto func =
        (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context.instance, "vkDestroyDebugUtilsMessengerEXT");
    func(context.instance, context.debug_messenger, context.allocator);
  }
#endif
  wen_debug("[Vulkan Backend] - Destroying Vulkan instance.");
  vkDestroyInstance(context.instance, context.allocator);
}

void vulkan_renderer_backend_resize(wen_renderer_backend_t* backend_, uint16_t width_, uint16_t height_) {
}

bool vulkan_renderer_backend_begin_frame(wen_renderer_backend_t* backend_, float deltatime_) {
  return true;
}

bool vulkan_renderer_backend_end_frame(wen_renderer_backend_t* backend_, float deltatime_) {
  return true;
}

VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT      message_severity,
                                                 VkDebugUtilsMessageTypeFlagsEXT             message_type,
                                                 const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                                                 void*                                       user_data) {
  switch (message_severity) {
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
    wen_error(callback_data->pMessage);
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
    wen_warn(callback_data->pMessage);
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
    wen_info(callback_data->pMessage);
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
    wen_trace(callback_data->pMessage);
    break;
  default: break;
  }
  return VK_FALSE;
}
