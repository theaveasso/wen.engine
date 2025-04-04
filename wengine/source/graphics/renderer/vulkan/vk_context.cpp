#include "vk_context.hpp"

#include <array>

#include "vk_options.hpp"
#include "vk_utils.hpp"
#include "vk_device.hpp"
#include "vk_debug.hpp"

#include <platform/wen_desktop_glfw.hpp>

static std::vector<const char *> vk_get_instance_extensions();

static VkInstance vk_instance_init(const char *app_name);

void vk_context_init(
    WenVkContext *vk_ctx,
    const WenWindow *window,
    const char *app_name
) {
    vk_ctx->instance        = vk_instance_init(app_name);
    vk_ctx->allocator       = nullptr;
    vk_ctx->physical_device = vk_physical_device_init(vk_ctx->instance);
    vk_ctx->surface         = vk_window_get_surface(window, vk_ctx->instance);
    vk_ctx->queue_families  = vk_queue_families_get(vk_ctx->physical_device, vk_ctx->surface);
    vk_ctx->logical_device  = vk_logical_device_init(vk_ctx->physical_device, &vk_ctx->queue_families);
    vk_ctx->queues          = vk_device_queues_init(vk_ctx->logical_device, &vk_ctx->queue_families);

    vk_debugger_init(&vk_ctx->debugger, vk_ctx->instance, vk_ctx->logical_device);
}

void vk_context_fini(
    WenVkContext *vk_ctx
) {
    vk_logical_device_fini(vk_ctx, vk_ctx->logical_device);

    vkDestroySurfaceKHR(
        vk_ctx->instance,
        vk_ctx->surface,
        vk_ctx->allocator
    );

    vk_debugger_fini(
        &vk_ctx->debugger,
        vk_ctx->instance
    );

    vkDestroyInstance(
        vk_ctx->instance,
        vk_ctx->allocator
    );
}

static VkInstance vk_instance_init(
    const char *app_name
) {
    VkApplicationInfo app_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
    app_info.pApplicationName   = app_name;
    app_info.pEngineName        = "WEN Engine";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion         = VK_API_VERSION_1_4;

    const std::vector<const char *> instance_extensions = vk_get_instance_extensions();

    VkInstanceCreateInfo instance_create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    instance_create_info.pApplicationInfo        = &app_info;
    instance_create_info.flags                   = 0;
    instance_create_info.enabledExtensionCount   = instance_extensions.size();
    instance_create_info.ppEnabledExtensionNames = instance_extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
    VkValidationFeaturesEXT            validation_features;
    std::array<const char *, 1>        validation_layers{};

    if (vk_opts::validation_enabled)
    {
        debug_create_info   = vk_messenger_info_init();
        validation_features = vk_validation_features_init();
        validation_layers   = vk_validation_layers_init();

        validation_features.pNext                = &debug_create_info;
        instance_create_info.pNext               = &validation_features;
        instance_create_info.enabledLayerCount   = validation_layers.size();
        instance_create_info.ppEnabledLayerNames = validation_layers.data();
    }

    VkInstance instance;
    vk_check(vkCreateInstance(
        &instance_create_info,
        nullptr,
        &instance
    ));
    return instance;
}

static std::vector<const char *> vk_get_instance_extensions() {
    uint32_t                  count;
    const char                **glfw_extensions = glfwGetRequiredInstanceExtensions(&count);
    std::vector<const char *> extensions;
    for (size_t               i                 = 0; i < count; ++i)
    {
        extensions.emplace_back(glfw_extensions[i]);
    }

    if (vk_opts::validation_enabled)
    {
        extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}