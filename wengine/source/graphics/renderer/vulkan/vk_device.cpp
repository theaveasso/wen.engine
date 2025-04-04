#include "vk_device.hpp"

#include <vector>
#include <stdexcept>
#include <optional>
#include <set>
#include <array>

#include "vk_utils.hpp"

VkPhysicalDevice vk_physical_device_init(
    VkInstance instance
) {
    uint32_t                      physical_device_count = 0;
    std::vector<VkPhysicalDevice> physical_devices;

    vk_check(vkEnumeratePhysicalDevices(
        instance,
        &physical_device_count,
        nullptr
    ));
    if (physical_device_count == 0)
    {
        throw std::runtime_error("failed to find a suitable device!");
    }

    physical_devices.resize(physical_device_count);
    vk_check(vkEnumeratePhysicalDevices(
        instance,
        &physical_device_count,
        physical_devices.data()
    ));

    VkPhysicalDevice physical_device = nullptr;
    for (auto        &device : physical_devices)
    {
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(device, &properties);

        if (properties.apiVersion < VK_API_VERSION_1_3) {
            continue;
        }
        VkPhysicalDeviceFeatures features{};
        vkGetPhysicalDeviceFeatures(device, &features);

        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            return device;
        }
        physical_device = device;
    }
    if (physical_device)
    {
        return physical_device;
    }

    throw std::runtime_error("failed to find a suitable GPU!");
}

WenDeviceQueueFamilies vk_queue_families_get(
    VkPhysicalDevice physical_device,
    VkSurfaceKHR surface
) {
    uint32_t                             family_property_count = 0;
    std::vector<VkQueueFamilyProperties> queue_family_properties;

    vkGetPhysicalDeviceQueueFamilyProperties(
        physical_device,
        &family_property_count,
        nullptr
    );

    if (family_property_count == 0)
    {
        throw std::runtime_error("failed to find device queue family!");
    }

    queue_family_properties.resize(family_property_count);
    vkGetPhysicalDeviceQueueFamilyProperties(
        physical_device,
        &family_property_count,
        queue_family_properties.data()
    );

    std::optional<uint32_t> graphics_index;
    std::optional<uint32_t> present_index;
    std::optional<uint32_t> transfer_index;
    std::optional<uint32_t> compute_index;

    // first pass: find graphics and present queues
    for (uint32_t i = 0; i < queue_family_properties.size(); ++i)
    {
        VkBool32 present_supported = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &present_supported);
        bool graphics_supported = queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT;

        if (present_supported)
        {
            present_index = i;
        }
        if (graphics_supported)
        {
            graphics_index = i;
        }
        if (present_index.has_value() && graphics_index.has_value())
        {
            break;
        }
    }
    if (!present_index.has_value() || !graphics_index.has_value())
    {
        throw std::runtime_error("failed to find suitable queue for graphics and present queues!");
    }

    // second pass: find compute queues
    for (uint32_t i = 0; i < queue_family_properties.size(); ++i)
    {
        bool compute_supported = queue_family_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT;

        if (compute_supported)
        {
            if (!compute_index.has_value() && (i != graphics_index.value()))
            {
                compute_index = i;
                break;
            }
        }
    }

    // third pass: find compute queues
    for (uint32_t i = 0; i < queue_family_properties.size(); ++i)
    {
        bool transform_supported = queue_family_properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT;

        if (transform_supported)
        {
            if (!transfer_index.has_value() && (i != graphics_index.value()))
            {
                transfer_index = i;
                break;
            }
        }
    }

    WenDeviceQueueFamilies queue_families{};
    queue_families.graphics = graphics_index.value();
    queue_families.present  = present_index.value();
    queue_families.transfer = transfer_index.has_value() ? transfer_index.value() : graphics_index.value();
    queue_families.compute  = compute_index.has_value() ? compute_index.value() : graphics_index.value();

    return queue_families;
}

VkDevice vk_logical_device_init(
    VkPhysicalDevice physical_device,
    const WenDeviceQueueFamilies *queue_families
) {
    std::set unique_family_indices
                 {queue_families->graphics, queue_families->present, queue_families->compute, queue_families->transfer};

    std::vector<VkDeviceQueueCreateInfo> queue_infos;
    constexpr float                      priority = 1.0f;

    for (const uint32_t &index : unique_family_indices)
    {
        VkDeviceQueueCreateInfo queue_create_info{VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
        queue_create_info.queueCount       = 1;
        queue_create_info.queueFamilyIndex = index;
        queue_create_info.pQueuePriorities = &priority;
        queue_infos.push_back(queue_create_info);
    }

    constexpr std::array device_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    VkPhysicalDeviceVulkan13Features features_1_3{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
    features_1_3.synchronization2 = VK_TRUE;
    features_1_3.dynamicRendering = VK_TRUE;

    VkPhysicalDeviceVulkan12Features features_1_2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};
    features_1_2.bufferDeviceAddress = VK_TRUE;
    features_1_2.pNext               = &features_1_3;

    VkPhysicalDeviceFeatures features_1_0{};
    features_1_0.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo device_create_info = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    device_create_info.pQueueCreateInfos       = queue_infos.data();
    device_create_info.queueCreateInfoCount    = queue_infos.size();
    device_create_info.ppEnabledExtensionNames = device_extensions.data();
    device_create_info.enabledExtensionCount   = device_extensions.size();
    device_create_info.pEnabledFeatures        = &features_1_0;
    device_create_info.pNext                   = &features_1_2;

    VkDevice device;
    vk_check(vkCreateDevice(
        physical_device,
        &device_create_info,
        nullptr,
        &device
    ));

    return device;
}

void vk_logical_device_fini(
    WenVkContext *vk_ctx,
    VkDevice device
) {
    vkDestroyDevice(
        device,
        vk_ctx->allocator
    );
}

WenDeviceQueues vk_device_queues_init(VkDevice device, const WenDeviceQueueFamilies *queue_families) {
    WenDeviceQueues queues{};
    vkGetDeviceQueue(device, queue_families->graphics, 0, &queues.graphics);
    vkGetDeviceQueue(device, queue_families->present, 0, &queues.present);
    vkGetDeviceQueue(device, queue_families->transfer, 0, &queues.transfer);
    vkGetDeviceQueue(device, queue_families->compute, 0, &queues.compute);

    return queues;
}

void vk_device_queues_fini(VkDevice device, const WenDeviceQueueFamilies *queue_families) {

}
