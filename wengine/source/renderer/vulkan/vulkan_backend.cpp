#include "wen/renderer/vulkan/vulkan_backend.hpp"
#include <SDL3/SDL.h>

#include "wen/renderer/vulkan/vulkan_types.inl"
#include "wen/renderer/vulkan/vulkan_utils.hpp"

#include "wen/core/wen_engine.hpp"
#include "wen/core/wen_logger.hpp"
#include "wen/core/wen_memory.hpp"
#include "wen/datastructures/wen_strbuf.hpp"
#include "wen/datastructures/wen_vec.hpp"
#include "wen/platform/wen_platform.hpp"

static wen_vulkan_context_t context;

static uint32_t cached_framebuffer_width  = 0;
static uint32_t cached_framebuffer_height = 0;

int32_t find_memory_index(uint32_t type_filter, uint32_t property_flags);

static void vulkan_instance_init(const char* app_name);
static void vulkan_instance_fini();

static void debug_utils_messenger_init();
static void debug_utils_messenger_fini();

static void vulkan_frame_buffers_init(wen_renderer_backend_t* backend);
static void vulkan_frame_buffers_reinit();
static void vulkan_frame_buffers_fini();

static void vulkan_command_buffers_init();
static void vulkan_command_buffers_fini();

static void vulkan_sync_objects_init();
static void vulkan_sync_objects_fini();

static void vulkan_get_required_extensions(const char*** required_extensions);
static bool vulkan_check_validation_layer_support(const char*** validation_layer_names, uint32_t* validation_layer_count);

static bool swapchain_reinit(wen_renderer_backend_t* backend);

VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data);

bool vulkan_renderer_backend_initialize(wen_renderer_backend_t* backend, const char* name_, struct wen_platform_state_t* state_) {
  context.find_memory_index = find_memory_index;

  context.allocator = nullptr;

  engine_get_framebuffer_size(&cached_framebuffer_width, &cached_framebuffer_height);
  context.framebuffer_width  = (cached_framebuffer_width != 0) ? cached_framebuffer_width : 800;
  context.framebuffer_height = (cached_framebuffer_height != 0) ? cached_framebuffer_height : 600;
  cached_framebuffer_width   = 0;
  cached_framebuffer_height  = 0;

  vulkan_instance_init(name_);
#if defined(WEN_DEBUG)
  debug_utils_messenger_init();
#endif
  if (!vulkan_surface_init(&context, state_)) {
    wen_error("[Vulkan Backend] - failed creating platform surface");
    return false;
  }

  /** Query and select devices. */
  if (!vulkan_device_init(&context)) {
    wen_error("[Vulkan Backend] - fail creating Vulkan devices.");
    return false;
  }

  vulkan_swapchain_init(&context, context.framebuffer_width, context.framebuffer_height, &context.swapchain);                                                                   /** Create swapchain. */
  vulkan_renderpass_init(&context, 0.0f, 0.0f, (float)context.framebuffer_width, (float)context.framebuffer_height, 0.0f, 0.0f, 0.2f, 1.0f, 1.0f, 0, &context.main_renderpass); /** Creating renderpass. */
  vulkan_frame_buffers_init(backend);                                                                                                                                           /** Creating swapchain framebuffer. */
  vulkan_command_buffers_init();                                                                                                                                                /** Creating command buffer. */
  vulkan_sync_objects_init();                                                                                                                                                   /** Creating sync object */

  return true;
}

void vulkan_renderer_backend_shutdown(wen_renderer_backend_t* backend_) {
  vkDeviceWaitIdle(context.devices.logical_device);
  vulkan_sync_objects_fini();
  vulkan_command_buffers_fini();
  vulkan_frame_buffers_fini();
  vulkan_renderpass_fini(&context, &context.main_renderpass);
  vulkan_swapchain_fini(&context, &context.swapchain);
  vulkan_device_fini(&context);
  vulkan_surface_fini(&context);
#if defined(WEN_DEBUG)
  debug_utils_messenger_fini();
#endif
  vulkan_instance_fini();
}

void vulkan_renderer_backend_resize(wen_renderer_backend_t* backend, uint16_t width, uint16_t height) {
  cached_framebuffer_width  = width;
  cached_framebuffer_height = height;
  context.framebuffer_size_generation++;
}

bool vulkan_renderer_backend_begin_frame(wen_renderer_backend_t* backend, float deltatime_) {
  wen_vulkan_device_t* device = &context.devices;

  if (context.recreating_swapchain) {
    VkResult result = vkDeviceWaitIdle(device->logical_device);
    if (!vk_result_is_success(result)) {
      wen_error("vulkan_renderer_backend_begin_frame vkDeviceWaitIdle");
      return false;
    }
    return false;
  }

  if (context.framebuffer_size_generation != context.framebuffer_size_last_generation) {
    VkResult result = vkDeviceWaitIdle(device->logical_device);
    if (!vk_result_is_success(result)) {
      wen_error("vulkan_renderer_backend_begin_frame vkDeviceWaitIdle");
      return false;
    }
    if (!swapchain_reinit(backend)) {
      return false;
    }
    wen_debug("Resize booting.");
    return false;
  }

  if (!vulkan_fence_wait(&context, &context.in_flight_fence[context.current_frame], UINT64_MAX)) {
    wen_warn("in-flight fence wait failure!");
    return false;
  }

  if (!vulkan_swapchain_acquire_next_image_index(&context, &context.swapchain, UINT64_MAX, context.image_available_semaphores[context.current_frame], 0, &context.image_index)) {
    return false;
  }

  // Begin recording command buffer;
  wen_vulkan_command_buffer_t* command_buffer = &context.graphics_command_buffers[context.image_index];
  vulkan_command_buffer_reset(command_buffer);
  vulkan_command_buffer_begin(command_buffer, false, false, false);

  // Dynamic state
  VkViewport viewport;
  viewport.x        = 0.0f;
  viewport.y        = (float)context.framebuffer_height;
  viewport.width    = (float)context.framebuffer_width;
  viewport.height   = -(float)context.framebuffer_height;
  viewport.minDepth = 0;
  viewport.maxDepth = 1;

  // Scissor
  VkRect2D scissor;
  scissor.offset.x = scissor.offset.y = 0;
  scissor.extent.width                = context.framebuffer_width;
  scissor.extent.height               = context.framebuffer_height;

  vkCmdSetViewport(command_buffer->handle, 0, 1, &viewport);
  vkCmdSetScissor(command_buffer->handle, 0, 1, &scissor);

  context.main_renderpass.w = (float)context.framebuffer_width;
  context.main_renderpass.h = (float)context.framebuffer_height;

  vulkan_renderpass_begin(command_buffer, &context.main_renderpass, context.swapchain.frame_buffers[context.image_index].handle);

  return true;
}

bool vulkan_renderer_backend_end_frame(wen_renderer_backend_t* backend_, float deltatime_) {
  wen_vulkan_command_buffer_t* command_buffer = &context.graphics_command_buffers[context.image_index];

  vulkan_renderpass_end(command_buffer, &context.main_renderpass);
  vulkan_command_buffer_end(command_buffer);

  if (context.images_in_flight[context.image_index] != VK_NULL_HANDLE) {
    vulkan_fence_wait(&context, context.images_in_flight[context.image_index], UINT64_MAX);
  }

  context.images_in_flight[context.image_index] = &context.in_flight_fence[context.current_frame];
  vulkan_fence_reset(&context, &context.in_flight_fence[context.current_frame]);

  VkSubmitInfo submit_info         = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
  submit_info.commandBufferCount   = 1;
  submit_info.pCommandBuffers      = &command_buffer->handle;
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores    = &context.queue_complete_semaphores[context.current_frame];
  submit_info.waitSemaphoreCount   = 1;
  submit_info.pWaitSemaphores      = &context.image_available_semaphores[context.current_frame];

  VkPipelineStageFlags flags[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submit_info.pWaitDstStageMask = flags;

  VkResult result = vkQueueSubmit(context.devices.graphics_queue, 1, &submit_info, context.in_flight_fence[context.current_frame].handle);
  if (result != VK_SUCCESS) {
    wen_error("vkQueueSubmit failed.");
    return false;
  }

  vulkan_command_buffer_update_submitted(command_buffer);
  vulkan_swapchain_present(&context, &context.swapchain, context.devices.graphics_queue, context.devices.present_queue, context.queue_complete_semaphores[context.current_frame], context.image_index);

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

void vulkan_frame_buffers_init(wen_renderer_backend_t* backend) {
  context.swapchain.frame_buffers = (wen_vulkan_framebuffer_t*)vec_reserve(wen_vulkan_framebuffer_t, context.swapchain.image_count);
  vulkan_frame_buffers_reinit();
}

void vulkan_frame_buffers_reinit() {
  for (uint32_t i = 0; i < context.swapchain.image_count; ++i) {
    uint32_t    attachment_count = 2;
    VkImageView attachments[]{
        context.swapchain.views[i],
        context.swapchain.depth_attachment.view};

    vulkan_framebuffer_init(
        &context,
        &context.main_renderpass,
        context.framebuffer_width,
        context.framebuffer_height,
        attachment_count,
        attachments,
        &context.swapchain.frame_buffers[i]);
  };
}

void vulkan_frame_buffers_fini() {
  for (uint32_t i = 0; i < context.swapchain.image_count; ++i) {
    vulkan_framebuffer_fini(&context, &context.swapchain.frame_buffers[i]);
  }
}

void vulkan_command_buffers_init() {
  if (!context.graphics_command_buffers) {
    context.graphics_command_buffers = (wen_vulkan_command_buffer_t*)vec_reserve(wen_vulkan_command_buffer_t, context.swapchain.image_count);
    for (uint32_t i = 0; i < context.swapchain.image_count; ++i) {
      wen_memzero(&context.graphics_command_buffers[i], sizeof(wen_vulkan_command_buffer_t));
    }
  }
  for (uint32_t i = 0; i < context.swapchain.image_count; ++i) {
    if (context.graphics_command_buffers[i].handle) {
      vulkan_command_buffer_free(
          &context,
          context.devices.graphics_command_pool,
          &context.graphics_command_buffers[i]);
    }
    wen_memzero(&context.graphics_command_buffers[i], sizeof(wen_vulkan_command_buffer_t));
    vulkan_command_buffer_allocate(
        &context,
        context.devices.graphics_command_pool,
        true,
        &context.graphics_command_buffers[i]);
  }
}

void vulkan_command_buffers_fini() {
  for (uint32_t i = 0; i < context.swapchain.image_count; ++i) {
    if (context.graphics_command_buffers[i].handle != nullptr) {
      vulkan_command_buffer_free(&context, context.devices.graphics_command_pool, &context.graphics_command_buffers[i]);
      context.graphics_command_buffers[i].handle = nullptr;
    }
  }
  vec_destroy(context.graphics_command_buffers);
  context.graphics_command_buffers = nullptr;
}

void vulkan_sync_objects_init() {
  context.image_available_semaphores = (VkSemaphore*)vec_reserve(VkSemaphore, context.swapchain.max_frames_in_flight);
  context.queue_complete_semaphores  = (VkSemaphore*)vec_reserve(VkSemaphore, context.swapchain.max_frames_in_flight);
  context.in_flight_fence            = (wen_vulkan_fence_t*)vec_reserve(wen_vulkan_fence_t, context.swapchain.max_frames_in_flight);

  for (uint8_t i = 0; i < context.swapchain.max_frames_in_flight; ++i) {
    VkSemaphoreCreateInfo semaphore_create_info = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    vk_check(vkCreateSemaphore(context.devices.logical_device, &semaphore_create_info, context.allocator, &context.image_available_semaphores[i]));
    vk_check(vkCreateSemaphore(context.devices.logical_device, &semaphore_create_info, context.allocator, &context.queue_complete_semaphores[i]));
    vulkan_fence_init(&context, true, &context.in_flight_fence[i]);
  }

  context.images_in_flight = (wen_vulkan_fence_t**)vec_reserve(wen_vulkan_fence_t, context.swapchain.max_frames_in_flight);
  for (uint32_t i = 0; i < context.swapchain.image_count; ++i) {
    context.images_in_flight[i] = nullptr;
  }
}

void vulkan_sync_objects_fini() {
  for (uint32_t i = 0; i < context.swapchain.max_frames_in_flight; ++i) {
    auto a = vec_len(context.image_available_semaphores);
    auto b = vec_len(context.queue_complete_semaphores);
    if (context.image_available_semaphores[i] != VK_NULL_HANDLE) {
      vkDestroySemaphore(context.devices.logical_device, context.image_available_semaphores[i], context.allocator);
      context.image_available_semaphores[i] = nullptr;
    }
    if (context.queue_complete_semaphores[i] != VK_NULL_HANDLE) {
      vkDestroySemaphore(context.devices.logical_device, context.queue_complete_semaphores[i], context.allocator);
      context.queue_complete_semaphores[i] = nullptr;
    }
    vulkan_fence_fini(&context, &context.in_flight_fence[i]);
  }
  vec_destroy(context.image_available_semaphores);
  vec_destroy(context.queue_complete_semaphores);
  vec_destroy(context.in_flight_fence);
  context.image_available_semaphores = nullptr;
  context.queue_complete_semaphores  = nullptr;
  context.in_flight_fence            = nullptr;
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

bool swapchain_reinit(wen_renderer_backend_t* backend) {
  if (context.recreating_swapchain) {
    return false;
  }
  if (context.framebuffer_width == 0 || context.framebuffer_height == 0) {
    return false;
  }
  context.recreating_swapchain = true;
  VkResult result              = vkDeviceWaitIdle(context.devices.logical_device);
  if (!vk_result_is_success(result)) {
    wen_error("swapchain_reinit vkDeviceWaitIdle");
    return false;
  }

  for (uint32_t i = 0; i < context.swapchain.image_count; ++i) {
    context.images_in_flight[i] = nullptr;
  }

  vulkan_device_query_swapchain_support(context.devices.physical_device, context.surface, &context.devices.swapchain_support_info);
  vulkan_device_detect_depth_format(&context.devices);

  vulkan_swapchain_reinit(&context, cached_framebuffer_width, cached_framebuffer_height, &context.swapchain);

  context.framebuffer_width  = cached_framebuffer_width;
  context.framebuffer_height = cached_framebuffer_height;
  context.main_renderpass.w  = (float)context.framebuffer_width;
  context.main_renderpass.h  = (float)context.framebuffer_height;
  cached_framebuffer_width   = 0;
  cached_framebuffer_height  = 0;

  context.framebuffer_size_last_generation = context.framebuffer_size_generation;

  for (uint32_t i = 0; i < context.swapchain.image_count; ++i) {
    vulkan_command_buffer_free(&context, context.devices.graphics_command_pool, &context.graphics_command_buffers[i]);
  }
  vulkan_frame_buffers_fini();

  context.main_renderpass.x = 0;
  context.main_renderpass.y = 0;
  context.main_renderpass.w = (float)context.framebuffer_width;
  context.main_renderpass.h = (float)context.framebuffer_height;

  vulkan_frame_buffers_reinit();
  vulkan_command_buffers_init();

  context.recreating_swapchain = false;
  return true;
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
