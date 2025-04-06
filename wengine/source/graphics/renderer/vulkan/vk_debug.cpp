#include "vk_debug.hpp"

static constexpr std::array enabled_validation_features{
    VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT};

static constexpr std::array disabled_validation_features{
    VK_VALIDATION_FEATURE_DISABLE_UNIQUE_HANDLES_EXT};

[[maybe_unused]] VkBool32
debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT           severity_flags,
    [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT message_types_flags,
    const VkDebugUtilsMessengerCallbackDataEXT      *pCallbackData,
    [[maybe_unused]] void                           *pUserData) {
    if (severity_flags == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
        WEN_TRACE("[Vulkan] {}", pCallbackData->pMessage);
    } else if (
        severity_flags == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        WEN_WARN("[Vulkan] {}", pCallbackData->pMessage);
    } else if (severity_flags == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        WEN_INFO("[Vulkan] {}", pCallbackData->pMessage);
    } else if (
        severity_flags == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        WEN_ERROR("[Vulkan] {}", pCallbackData->pMessage);
    }

    return VK_FALSE;
}

VkResult
vk_debugger_init(
    WenVkDebugger *debugger, VkInstance instance, VkDevice device) {
    debugger->logical_device = device;

    auto msg_fn = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));

    VkDebugUtilsMessengerCreateInfoEXT messenger_create_info =
        vk_messenger_info_init();
    if (msg_fn != nullptr) {
        debugger->obj_name_pfn =
            reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(
                vkGetInstanceProcAddr(
                    instance, "vkSetDebugUtilsObjectNameEXT"));

        VkResult result = msg_fn(
            instance, &messenger_create_info, nullptr, &debugger->messenger);

        return result;
    }

    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void
vk_debugger_fini(const WenVkDebugger *debugger, VkInstance instance) {
    auto destroy_messenger_pfn =
        reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance, "vkGetDebugUtilsMessengerEXT"));
    if (destroy_messenger_pfn) {
        destroy_messenger_pfn(instance, debugger->messenger, nullptr);
    }
}

VkDebugUtilsMessengerCreateInfoEXT
vk_messenger_info_init() {
    VkDebugUtilsMessengerCreateInfoEXT messenger_create_info = {
        VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};

    uint32_t message_severity = 0;
    message_severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
    message_severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
    message_severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    messenger_create_info.messageSeverity = message_severity;

    uint32_t message_types = 0;
    message_types |= VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
    message_types |= VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    message_types |= VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    messenger_create_info.messageType = message_types;

    messenger_create_info.pfnUserCallback = debug_callback;
    return messenger_create_info;
}

VkValidationFeaturesEXT
vk_validation_features_init() {
    VkValidationFeaturesEXT validation_features = {
        VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT};
    validation_features.enabledValidationFeatureCount =
        enabled_validation_features.size();
    validation_features.pEnabledValidationFeatures =
        enabled_validation_features.data();
    validation_features.disabledValidationFeatureCount =
        disabled_validation_features.size();
    validation_features.pDisabledValidationFeatures =
        disabled_validation_features.data();

    return validation_features;
}

std::array<const char *, 1>
vk_validation_layers_init() {
    return {"VK_LAYER_KHRONOS_validation"};
}
