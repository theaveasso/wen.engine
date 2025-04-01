#include "wen/renderer/vulkan/vulkan_device.hpp"

#include "wen/core/wen_logger.hpp"
#include "wen/core/wen_memory.hpp"

#include "wen/datastructures/wen_strbuf.hpp"
#include "wen/datastructures/wen_vec.hpp"

typedef struct wen_vulkan_device_extensions_t {
  const char** names;
  uint32_t     count;
} wen_vulkan_device_extensions_t;

typedef struct wen_vulkan_physical_device_requirements_t {
  bool graphics;
  bool present;
  bool compute;
  bool transfer;

  bool sampler_anisotropy;
  bool discrete_gpu;

  wen_vulkan_device_extensions_t device_extensions;
} wen_vulkan_physical_device_requirements_t;

typedef struct wen_vulkan_physical_device_queue_family_info_t {
  int32_t graphics_family_index;
  int32_t present_family_index;
  int32_t compute_family_index;
  int32_t transfer_family_index;
} wen_vulkan_physical_device_queue_family_info_t;

bool select_physical_device(wen_vulkan_context_t* context_);

bool physical_device_meets_requirement(VkPhysicalDevice device_, VkSurfaceKHR surface_, const VkPhysicalDeviceProperties* props_, const VkPhysicalDeviceFeatures* features_, const wen_vulkan_physical_device_requirements_t* requirements, wen_vulkan_physical_device_queue_family_info_t* out_queue_info_, wen_vulkan_swapchain_support_info_t* out_swapchain_support);

bool vulkan_device_init(wen_vulkan_context_t* context_) {
  if (!select_physical_device(context_)) {
    wen_trace("");
    return false;
  }

  bool present_shared_graphics_queue =
      context_->devices.graphics_queue_index == context_->devices.present_queue_index;
  bool transfer_shared_graphics_queue =
      context_->devices.graphics_queue_index == context_->devices.transfer_queue_index;

  uint32_t index_count = 1;
  if (!present_shared_graphics_queue) {
    index_count++;
  }
  if (!transfer_shared_graphics_queue) {
    index_count++;
  }
  uint32_t indices[index_count];

  uint8_t index    = 0;
  indices[index++] = context_->devices.graphics_queue_index;
  if (!present_shared_graphics_queue) {
    indices[index++] = context_->devices.present_queue_index;
  }
  if (!transfer_shared_graphics_queue) {
    indices[index++] = context_->devices.transfer_queue_index;
  }

  VkDeviceQueueCreateInfo queue_create_infos[index_count];
  for (uint32_t i = 0; i < index_count; ++i) {
    queue_create_infos[i].sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_infos[i].queueFamilyIndex = indices[i];
    queue_create_infos[i].queueCount       = 1;

    queue_create_infos[i].flags = 0;
    queue_create_infos[i].pNext = nullptr;

    float queue_priorities[2] = {1.0f, 1.0f};

    if (indices[i] == context_->devices.graphics_queue_index) {
      queue_create_infos[i].queueCount       = 2;
      queue_create_infos[i].pQueuePriorities = queue_priorities;
    } else {
      queue_create_infos[i].queueCount       = 1;
      queue_create_infos[i].pQueuePriorities = &queue_priorities[0];
    }
  }

  VkPhysicalDeviceFeatures device_features = {};
  device_features.samplerAnisotropy        = VK_TRUE;

  VkDeviceCreateInfo device_create_info      = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
  device_create_info.queueCreateInfoCount    = index_count;
  device_create_info.pQueueCreateInfos       = queue_create_infos;
  device_create_info.pEnabledFeatures        = &device_features;
  device_create_info.enabledExtensionCount   = 1;
  const char* extension_names                = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
  device_create_info.ppEnabledExtensionNames = &extension_names;

  vk_check(vkCreateDevice(
      context_->devices.physical_device,
      &device_create_info,
      context_->allocator,
      &context_->devices.logical_device));

  vkGetDeviceQueue(
      context_->devices.logical_device,
      context_->devices.graphics_queue_index,
      0,
      &context_->devices.graphics_queue);

  vkGetDeviceQueue(
      context_->devices.logical_device,
      context_->devices.graphics_queue_index,
      0,
      &context_->devices.present_queue);

  vkGetDeviceQueue(
      context_->devices.logical_device,
      context_->devices.graphics_queue_index,
      0,
      &context_->devices.transfer_queue);

  return true;
}

void vulkan_device_fini(wen_vulkan_context_t* context_) {
  context_->devices.graphics_queue = nullptr;
  context_->devices.present_queue  = nullptr;
  context_->devices.transfer_queue = nullptr;

  if (context_->devices.logical_device) {
    vkDestroyDevice(context_->devices.logical_device, context_->allocator);
    context_->devices.logical_device = nullptr;
  }

  context_->devices.physical_device = nullptr;

  if (context_->devices.swapchain_support_info.formats) {
    wen_memfree(context_->devices.swapchain_support_info.formats,
                sizeof(VkSurfaceFormatKHR) * context_->devices.swapchain_support_info.format_count,
                MEMORY_TAG_RENDERER);
    context_->devices.swapchain_support_info.formats      = nullptr;
    context_->devices.swapchain_support_info.format_count = 0;
  }
  if (context_->devices.swapchain_support_info.present_modes) {
    wen_memfree(context_->devices.swapchain_support_info.present_modes,
                sizeof(VkPresentModeKHR) * context_->devices.swapchain_support_info.present_mode_count,
                MEMORY_TAG_RENDERER);
    context_->devices.swapchain_support_info.present_modes      = nullptr;
    context_->devices.swapchain_support_info.present_mode_count = 0;
  }
  wen_memzero(&context_->devices.swapchain_support_info, sizeof(context_->devices.swapchain_support_info));
}

void vulkan_logical_device_init(wen_vulkan_context_t* context) {
}

void vulkan_logical_device_fini(wen_vulkan_context_t* context) {
}

void vulkan_device_query_swapchain_support(
    VkPhysicalDevice                     device,
    VkSurfaceKHR                         surface,
    wen_vulkan_swapchain_support_info_t* out_swapchain_support) {

  vk_check(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
      device,
      surface,
      &out_swapchain_support->capabilities));
  vk_check(vkGetPhysicalDeviceSurfaceFormatsKHR(
      device,
      surface,
      &out_swapchain_support->format_count,
      nullptr));

  if (out_swapchain_support->format_count != 0) {
    if (!out_swapchain_support->formats) {
      out_swapchain_support->formats = (VkSurfaceFormatKHR*)wen_memalloc(sizeof(VkSurfaceFormatKHR) * out_swapchain_support->format_count, MEMORY_TAG_RENDERER);
    }
    vk_check(vkGetPhysicalDeviceSurfaceFormatsKHR(
        device,
        surface,
        &out_swapchain_support->format_count,
        out_swapchain_support->formats));
  }

  vk_check(vkGetPhysicalDeviceSurfacePresentModesKHR(
      device,
      surface,
      &out_swapchain_support->present_mode_count,
      nullptr));

  if (out_swapchain_support->present_mode_count != 0) {
    if (!out_swapchain_support->present_modes) {
      out_swapchain_support->present_modes =
          (VkPresentModeKHR*)wen_memalloc(
              sizeof(VkPresentModeKHR) * out_swapchain_support->present_mode_count,
              MEMORY_TAG_RENDERER);
    }
    vk_check(vkGetPhysicalDeviceSurfacePresentModesKHR(
        device,
        surface,
        &out_swapchain_support->present_mode_count,
        out_swapchain_support->present_modes));
  }
}

bool vulkan_device_detect_depth_format(wen_vulkan_device_t* device) {
  VkFormat candidates[3]{
      VK_FORMAT_D32_SFLOAT,
      VK_FORMAT_D32_SFLOAT_S8_UINT,
      VK_FORMAT_D24_UNORM_S8_UINT};

  uint32_t flags = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
  for (auto& candidate : candidates) {
    VkFormatProperties properties;
    vkGetPhysicalDeviceFormatProperties(
        device->physical_device,
        candidate,
        &properties);

    if ((properties.linearTilingFeatures & flags) == flags) {
      device->depth_format = candidate;
      return true;
    } else if ((properties.optimalTilingFeatures & flags) == flags) {
      device->depth_format = candidate;
      return true;
    }
  }
  return false;
}

bool select_physical_device(wen_vulkan_context_t* context_) {
  uint32_t physical_device_count = 0;
  vkEnumeratePhysicalDevices(context_->instance, &physical_device_count, nullptr);
  if (physical_device_count == 0) {
    return false;
  }
  VkPhysicalDevice physical_devices[physical_device_count];
  vkEnumeratePhysicalDevices(context_->instance, &physical_device_count, physical_devices);
  for (uint32_t i = 0; i < physical_device_count; ++i) {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physical_devices[i], &properties);

    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(physical_devices[i], &features);

    VkPhysicalDeviceMemoryProperties memory;
    vkGetPhysicalDeviceMemoryProperties(physical_devices[i], &memory);

    // TODO: should be require by engine configuration;
    wen_vulkan_physical_device_requirements_t requirements = {};
    requirements.graphics                                  = true;
    requirements.present                                   = true;
    requirements.transfer                                  = true;
    // NOTE: Enable this if compute will be required.
    //    requirements.compute = true;
    requirements.sampler_anisotropy = true;
    requirements.discrete_gpu       = true;

    requirements.device_extensions       = {};
    requirements.device_extensions.count = 1;
    requirements.device_extensions.names = (const char**)wen_memalloc(
        sizeof(const char*) * requirements.device_extensions.count,
        MEMORY_TAG_RENDERER);
    requirements.device_extensions.names[0] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

    wen_vulkan_physical_device_queue_family_info_t queue_info = {};

    bool result = physical_device_meets_requirement(
        physical_devices[i],
        context_->surface,
        &properties,
        &features, &requirements,
        &queue_info,
        &context_->devices.swapchain_support_info);

    if (result) {
      wen_info("Selected device: %s", properties.deviceName);
      switch (properties.deviceType) {
      case VK_PHYSICAL_DEVICE_TYPE_OTHER:
        wen_info("GPU type is Unknown");
        break;
      case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        wen_info("GPU type is Discrete");
        break;
      case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
        wen_info("GPU type is Virtual");
        break;
      case VK_PHYSICAL_DEVICE_TYPE_CPU:
        wen_info("GPU type is CPU");
        break;
      case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        wen_info("GPU type is Integrated");
        break;
      default:
        break;
      }
      wen_info("GPU Driver version: %d.%d.%d",
               VK_VERSION_MAJOR(properties.driverVersion),
               VK_VERSION_MINOR(properties.driverVersion),
               VK_VERSION_PATCH(properties.driverVersion));
      wen_info("Vulkan API version: %d.%d.%d",
               VK_VERSION_MAJOR(properties.apiVersion),
               VK_VERSION_MINOR(properties.apiVersion),
               VK_VERSION_PATCH(properties.apiVersion));

      context_->devices.physical_device      = physical_devices[i];
      context_->devices.graphics_queue_index = queue_info.graphics_family_index;
      context_->devices.present_queue_index  = queue_info.present_family_index;
      //      context_->devices.compute_queue_index = queue_info.compute_family_index;
      context_->devices.transfer_queue_index = queue_info.transfer_family_index;
      context_->devices.properties           = properties;
      context_->devices.features             = features;
      context_->devices.memory               = memory;

      break;
    }
  }

  if (!context_->devices.physical_device) {
    wen_error("[Vulkan Backend] - No physical devices were found which meet the requirements.");
    return false;
  }

  return true;
}

bool physical_device_meets_requirement(
    VkPhysicalDevice                                 device_,
    VkSurfaceKHR                                     surface_,
    const VkPhysicalDeviceProperties*                props_,
    const VkPhysicalDeviceFeatures*                  features_,
    const wen_vulkan_physical_device_requirements_t* requirements_,
    wen_vulkan_physical_device_queue_family_info_t*  out_queue_info_,
    wen_vulkan_swapchain_support_info_t*             out_swapchain_support_) {

  out_queue_info_->graphics_family_index = -1;
  out_queue_info_->present_family_index  = -1;
  out_queue_info_->compute_family_index  = -1;
  out_queue_info_->transfer_family_index = -1;

  if (requirements_->discrete_gpu) {
    if (props_->deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      return false;
    }
  }

  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device_, &queue_family_count, nullptr);
  VkQueueFamilyProperties queue_families[queue_family_count];
  vkGetPhysicalDeviceQueueFamilyProperties(device_, &queue_family_count, queue_families);

  wen_info("| Graphics | Present | Compute | Transfer | Name |");
  uint8_t min_transfer_score = 255;
  for (int32_t i = 0; i < queue_family_count; ++i) {
    uint8_t current_transfer_score = 0;

    if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      out_queue_info_->graphics_family_index = i;
      ++current_transfer_score;
    }
    if (queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
      out_queue_info_->compute_family_index = i;
      ++current_transfer_score;
    }
    if (queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
      if (current_transfer_score <= min_transfer_score) {
        min_transfer_score                     = current_transfer_score;
        out_queue_info_->transfer_family_index = i;
      }
    }

    VkBool32 support_present = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(device_, i, surface_, &support_present);
    if (support_present) {
      out_queue_info_->present_family_index = i;
    }
  }
  wen_info(" %d | %d | %d | %d| %s", out_queue_info_->graphics_family_index != -1, out_queue_info_->present_family_index != -1, out_queue_info_->compute_family_index != -1, out_queue_info_->transfer_family_index != -1, props_->deviceName);

  if (
      (!requirements_->graphics || (requirements_->graphics && out_queue_info_->graphics_family_index != -1)) &&
      (!requirements_->present || (requirements_->present && out_queue_info_->present_family_index != -1)) &&
      (!requirements_->compute || (requirements_->compute && out_queue_info_->compute_family_index != -1)) &&
      (!requirements_->transfer || (requirements_->transfer && out_queue_info_->transfer_family_index != -1))) {
    wen_info("Device meets queue requirements.");
    wen_info("Graphics family index: %i", out_queue_info_->graphics_family_index);
    wen_info("Present family index: %i", out_queue_info_->present_family_index);
    wen_info("Compute family index: %i", out_queue_info_->compute_family_index);
    wen_info("Transfer family index: %i", out_queue_info_->transfer_family_index);

    vulkan_device_query_swapchain_support(device_, surface_, out_swapchain_support_);
    if (out_swapchain_support_->format_count < 1 || out_swapchain_support_->present_mode_count < 1) {
      if (out_swapchain_support_->formats) {
        wen_memfree(out_swapchain_support_->formats, sizeof(VkSurfaceFormatKHR) * out_swapchain_support_->format_count, MEMORY_TAG_RENDERER);
      }
      if (out_swapchain_support_->present_modes) {
        wen_memfree(out_swapchain_support_->present_modes, sizeof(VkPresentModeKHR) * out_swapchain_support_->present_mode_count, MEMORY_TAG_RENDERER);
      }
      wen_info("[Vulkan Backend] - Required swapchain support not present, skipping device.");
      return false;
    }

    if (requirements_->device_extensions.count > 0) {
      uint32_t               available_extension_count = 0;
      VkExtensionProperties* available_extension       = nullptr;

      vk_check(vkEnumerateDeviceExtensionProperties(device_, nullptr, &available_extension_count, nullptr));
      if (available_extension_count != 0) {
        available_extension = (VkExtensionProperties*)wen_memalloc(sizeof(VkExtensionProperties) * available_extension_count, MEMORY_TAG_RENDERER);
        vk_check(vkEnumerateDeviceExtensionProperties(device_, nullptr, &available_extension_count, available_extension));

//        uint32_t required_extension_count = vec_len(available_extension);
        for (uint32_t i = 0; i < requirements_->device_extensions.count; ++i) {
          bool found = false;
          for (uint32_t j = 0; j < available_extension_count; ++j) {
            if (wen_str_eq(requirements_->device_extensions.names[i], available_extension[j].extensionName)) {
              wen_info("found");
              wen_info(requirements_->device_extensions.names[i]);
              found = true;
              break;
            }
          }
          if (!found) {
            wen_info("[Vulkan Backend] - Required extensions not found %s skipping device.", requirements_->device_extensions.names[i]);
            return false;
          }
        }

        wen_memfree(available_extension, sizeof(VkExtensionProperties) * available_extension_count, MEMORY_TAG_RENDERER);
      }
    }

    if (requirements_->sampler_anisotropy && !features_->samplerAnisotropy) {
      wen_info("Device does not support samplerAnisotropy, skipping.");
      return false;
    }
  }

  return true;
}
