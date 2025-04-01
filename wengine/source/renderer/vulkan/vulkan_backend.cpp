#include "wen/renderer/vulkan/vulkan_backend.hpp"


#include "wen/renderer/vulkan/vulkan_types.inl"
#include "wen/renderer/vulkan/vulkan_utils.hpp"

#include "wen/core/wen_logger.hpp"
#include "wen/datastructures/wen_strbuf.hpp"
#include "wen/datastructures/wen_vec.hpp"
#include "wen/platform/wen_platform.hpp"

static wen_vulkan_context_t context;

int32_t find_memory_index(uint32_t type_filter, uint32_t property_flags);

void vulkan_instance_init(const char* app_name);
void vulkan_instance_fini();

void debug_utils_messenger_init();
void debug_utils_messenger_fini();

void vulkan_command_buffers_init(wen_renderer_backend_t* backend);
void vulkan_command_buffers_fini(wen_renderer_backend_t* backend);

void vulkan_get_required_extensions(const char*** required_extensions);
bool vulkan_check_validation_layer_support(const char*** validation_layer_names, uint32_t* validation_layer_count);

VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data);

bool vulkan_renderer_backend_initialize(wen_renderer_backend_t* backend_, const char* name_, struct wen_platform_state_t* state_) {
  context.find_memory_index = find_memory_index;
  context.allocator         = nullptr;

  vulkan_instance_init(name_);

#if defined(WEN_DEBUG)
  debug_utils_messenger_init();
#endif

  /** Creating surface. */
  if (!vulkan_surface_init(&context, state_)) {
    wen_error("[Vulkan Backend] - failed creating platform surface");
    return false;
  }

  /** Query and select devices. */
  if (!vulkan_device_init(&context)) {
    wen_error("[Vulkan Backend] - fail creating Vulkan devices.");
    return false;
  }

  /** Create swapchain. */
  vulkan_swapchain_init(
      &context,
      context.framebuffer_width,
      context.framebuffer_height,
      &context.swapchain);

  vulkan_renderpass_init(
      &context,
      0.0f, 0.0f, (float)context.framebuffer_width, (float)context.framebuffer_height,
      0.0f, 0.0f, 0.2f, 1.0f, 1.0f, 0,
      &context.main_renderpass);

  return true;
}

void vulkan_renderer_backend_shutdown(wen_renderer_backend_t* backend_) {
  vulkan_renderpass_fini(&context, &context.main_renderpass);
  vulkan_swapchain_fini(&context, &context.swapchain);
  vulkan_device_fini(&context);
  vulkan_surface_fini(&context);
#if defined(WEN_DEBUG)
  debug_utils_messenger_fini();
#endif
  vulkan_instance_fini();
}

void vulkan_renderer_backend_resize(wen_renderer_backend_t* backend_, uint16_t width_, uint16_t height_) {
}

bool vulkan_renderer_backend_begin_frame(wen_renderer_backend_t* backend_, float deltatime_) {
  return true;
}

bool vulkan_renderer_backend_end_frame(wen_renderer_backend_t* backend_, float deltatime_) {
  return true;
}

void vulkan_instance_init(const char* app_name) {
  VkApplicationInfo app_info  = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
  app_info.apiVersion         = VK_API_VERSION_1_4;
  app_info.pApplicationName   = app_name;
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 1);
  app_info.pEngineName        = "Wen Engine";
  app_info.engineVersion      = VK_MAKE_VERSION(1, 0, 0);

  VkInstanceCreateInfo create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
  create_info.pApplicationInfo     = &app_info;

  /** Obtain a list of required extensions. */
  const char** required_extensions = vec_create(const char*);
  vulkan_get_required_extensions(&required_extensions);

  const char** required_validation_layer_names = nullptr;
  uint32_t     required_validation_layer_count = 0;
#if defined(WEN_DEBUG)
  vulkan_check_validation_layer_support(&required_validation_layer_names, &required_validation_layer_count);
#endif

  create_info.enabledExtensionCount   = vec_len(required_extensions);
  create_info.ppEnabledExtensionNames = required_extensions;
  create_info.enabledLayerCount       = required_validation_layer_count;
  create_info.ppEnabledLayerNames     = required_validation_layer_names;

  vk_check(vkCreateInstance(&create_info, context.allocator, &context.instance));
  wen_debug("vulkan instance init");
}

void vulkan_instance_fini() {
  vkDestroyInstance(context.instance, context.allocator);
}

void vulkan_get_required_extensions(const char*** required_extensions) {
  platform_get_required_exts_names(required_extensions);
#if defined(WEN_DEBUG)
  vec_push(*required_extensions, &VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  wen_debug("[VulkanBackend] - instance ppEnabledExtensionNames");
  for (int i = 0; i < vec_len(*required_extensions); ++i) {
    wen_debug((*required_extensions)[i]);
  }
#endif
}

bool vulkan_check_validation_layer_support(const char*** validation_layer_names, uint32_t* validation_layer_count) {
#if defined(WEN_DEBUG)
  *validation_layer_names = (const char**)vec_create(const char*);
  vec_push(*validation_layer_names, &"VK_LAYER_KHRONOS_validation");
  *validation_layer_count = vec_len(*validation_layer_names);

  // Obtain a list of available validation layers
  uint32_t available_layer_count = 0;
  vkEnumerateInstanceLayerProperties(&available_layer_count, nullptr);
  auto* available_layers = (VkLayerProperties*)vec_reserve(VkLayerProperties, available_layer_count);
  vkEnumerateInstanceLayerProperties(&available_layer_count, available_layers);

  for (uint32_t i = 0; i < *validation_layer_count; ++i) {
    bool found = false;
    for (uint32_t j = 0; j < available_layer_count; ++j) {
      if (wen_str_eq((*validation_layer_names)[i], available_layers[j].layerName)) {
        found = true;
        wen_debug("[Vulkan Backend] - Searching for layer %s found!", (*validation_layer_names)[i]);
        break;
      }
    }
    if (!found) {
      wen_trace("[Vulkan Backend] - Required validation layer %s missing!", (*validation_layer_names)[i]);
      return false;
    }
#endif
  }
  return true;
}

void vulkan_command_buffers_init(wen_renderer_backend_t* backend) {
//  if (!context.graphics_command_buffers) {
//    context.graphics_command_buffers = (wen_vulkan_command_buffer_t*)vec_reserve(wen_vulkan_command_buffer_t, context.swapchain.image_count);
//    for (uint32_t i = 0; i < context.swapchain.image_count; ++i) {
//      wen_memzero(&context.graphics_command_buffers[i], sizeof(wen_vulkan_command_buffer_t));
//    }
//  }
//  for (uint32_t i = 0; i < context.swapchain.image_count; ++i) {
//    if (context.graphics_command_buffers[i].handle) {
//      vulkan_command_buffer_free(
//          &context,
//          context.devices.graphics_command_pool,
//          &context.graphics_command_buffers[i]);
//    }
//  }
}

void vulkan_command_buffers_fini(wen_renderer_backend_t* backend) {
}

void debug_utils_messenger_init() {
  uint32_t log_severity = 0;
  log_severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  log_severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
  log_severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
  log_severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;

  uint32_t message_type = 0;
  message_type |= VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
  message_type |= VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
  message_type |= VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

  VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
  debug_create_info.messageSeverity                    = log_severity;
  debug_create_info.messageType                        = message_type;
  debug_create_info.pfnUserCallback                    = vk_debug_callback;

  wen_debug("[Vulkan] - vkCreateDebugUtilsMessengerEXT.");
  auto debug_utils_messenger_create_proc =
      (PFN_vkCreateDebugUtilsMessengerEXT)
          vkGetInstanceProcAddr(context.instance, "vkCreateDebugUtilsMessengerEXT");
  debug_utils_messenger_create_proc(
      context.instance,
      &debug_create_info,
      context.allocator,
      &context.debug_messenger);
};

void debug_utils_messenger_fini() {
  if (!context.debug_messenger) {
    return;
  }
  wen_debug("[Vulkan] - vkDestroyDebugUtilsMessengerEXT.");
  auto debug_utils_messenger_destroy_proc =
      (PFN_vkDestroyDebugUtilsMessengerEXT)
          vkGetInstanceProcAddr(context.instance, "vkDestroyDebugUtilsMessengerEXT");
  debug_utils_messenger_destroy_proc(
      context.instance,
      context.debug_messenger,
      context.allocator);
};

VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void*) {
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
int32_t find_memory_index(uint32_t type_filter, uint32_t property_flags) {
  VkPhysicalDeviceMemoryProperties memory_properties;
  vkGetPhysicalDeviceMemoryProperties(context.devices.physical_device, &memory_properties);

  for (int32_t i = 0; i < memory_properties.memoryTypeCount; ++i) {
    if (type_filter & (1 << i) && (memory_properties.memoryTypes[i].propertyFlags & property_flags) == property_flags) {
      return i;
    }
  }
  wen_warn("find_memory_index - unable to find suitable memory type");
  return -1;
}
