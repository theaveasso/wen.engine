#pragma once

#include <glm/vec4.hpp>
#include <volk.h>

#include "wvk/common/wvk_error.hpp"
#include "wvk/common/wvk_pch.hpp"

namespace wvk::core {

    struct ImageData;
}

namespace wvk::gfx {
#pragma region Vulkan Util

    inline const char *wvk_vk_err_string(VkResult result) {
        switch (result) {
            case VK_SUCCESS:
                return "VK_SUCCESS";
            case VK_NOT_READY:
                return "VK_NOT_READY";
            case VK_TIMEOUT:
                return "VK_TIMEOUT";
            case VK_EVENT_SET:
                return "VK_EVENT_SET";
            case VK_EVENT_RESET:
                return "VK_EVENT_RESET";
            case VK_INCOMPLETE:
                return "VK_INCOMPLETE";
            case VK_ERROR_OUT_OF_HOST_MEMORY:
                return "VK_ERROR_OUT_OF_HOST_MEMORY";
            case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
            case VK_ERROR_INITIALIZATION_FAILED:
                return "VK_ERROR_INITIALIZATION_FAILED";
            case VK_ERROR_DEVICE_LOST:
                return "VK_ERROR_DEVICE_LOST";
            case VK_ERROR_MEMORY_MAP_FAILED:
                return "VK_ERROR_MEMORY_MAP_FAILED";
            case VK_ERROR_LAYER_NOT_PRESENT:
                return "VK_ERROR_LAYER_NOT_PRESENT";
            case VK_ERROR_EXTENSION_NOT_PRESENT:
                return "VK_ERROR_EXTENSION_NOT_PRESENT";
            case VK_ERROR_FEATURE_NOT_PRESENT:
                return "VK_ERROR_FEATURE_NOT_PRESENT";
            case VK_ERROR_INCOMPATIBLE_DRIVER:
                return "VK_ERROR_INCOMPATIBLE_DRIVER";
            case VK_ERROR_TOO_MANY_OBJECTS:
                return "VK_ERROR_TOO_MANY_OBJECTS";
            case VK_ERROR_FORMAT_NOT_SUPPORTED:
                return "VK_ERROR_FORMAT_NOT_SUPPORTED";
            case VK_ERROR_FRAGMENTED_POOL:
                return "VK_ERROR_FRAGMENTED_POOL";
            case VK_ERROR_UNKNOWN:
                return "VK_ERROR_UNKNOWN";
            case VK_ERROR_OUT_OF_POOL_MEMORY:
                return "VK_ERROR_OUT_OF_POOL_MEMORY";
            case VK_ERROR_INVALID_EXTERNAL_HANDLE:
                return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
            case VK_ERROR_FRAGMENTATION:
                return "VK_ERROR_FRAGMENTATION";
            case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
                return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
            case VK_PIPELINE_COMPILE_REQUIRED:
                return "VK_PIPELINE_COMPILE_REQUIRED";
            case VK_ERROR_SURFACE_LOST_KHR:
                return "VK_ERROR_SURFACE_LOST_KHR";
            case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
                return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
            case VK_SUBOPTIMAL_KHR:
                return "VK_SUBOPTIMAL_KHR";
            case VK_ERROR_OUT_OF_DATE_KHR:
                return "VK_ERROR_OUT_OF_DATE_KHR";
            case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
                return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
            case VK_ERROR_VALIDATION_FAILED_EXT:
                return "VK_ERROR_VALIDATION_FAILED_EXT";
            case VK_ERROR_INVALID_SHADER_NV:
                return "VK_ERROR_INVALID_SHADER_NV";
#ifdef VK_ENABLE_BETA_EXTENSIONS
                case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR:
            return "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR";
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
                case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR:
            return "VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR";
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
                case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR:
            return "VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR";
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
                case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR:
            return "VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR";
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
                case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR:
            return "VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR";
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
                case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR:
            return "VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR";
#endif
            case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
                return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
            case VK_ERROR_NOT_PERMITTED_KHR:
                return "VK_ERROR_NOT_PERMITTED_KHR";
            case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
                return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
            case VK_THREAD_IDLE_KHR:
                return "VK_THREAD_IDLE_KHR";
            case VK_THREAD_DONE_KHR:
                return "VK_THREAD_DONE_KHR";
            case VK_OPERATION_DEFERRED_KHR:
                return "VK_OPERATION_DEFERRED_KHR";
            case VK_OPERATION_NOT_DEFERRED_KHR:
                return "VK_OPERATION_NOT_DEFERRED_KHR";
            case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:
                return "VK_ERROR_COMPRESSION_EXHAUSTED_EXT";
            case VK_RESULT_MAX_ENUM:
                return "VK_RESULT_MAX_ENUM";
            default:
                return "??????";
        }
    }

#define vk_assert(func)                                                         \
    do                                                                          \
    {                                                                           \
        const VkResult vk_assert_result = func;                                 \
        if (vk_assert_result != VK_SUCCESS)                                     \
        {                                                                       \
            WVK_WARN("Vulkan API call failed: {} {} {} {}", __FILE__, __LINE__, \
                     #func, wvk_vk_err_string(vk_assert_result));               \
            assert(false);                                                      \
        }                                                                       \
    } while (0)

/** @def vk_assert_return macro for validate Vulkan Api function call and
 * returns the control to the calling code.*/
#define vk_assert_return(func)                                                  \
    do                                                                          \
    {                                                                           \
        const VkResult vk_assert_result = func;                                 \
        if (vk_assert_result != VK_SUCCESS)                                     \
        {                                                                       \
            WVK_WARN("Vulkan API call failed: {} {} {} {}", __FILE__, __LINE__, \
                     #func, wvk_vk_err_string(vk_assert_result));               \
            assert(false);                                                      \
            return wvk_vk_err_string(vk_assert_result);                         \
        }                                                                       \
    } while (0)
#if defined(WVK_DBG)
    void WVK_API cmd_begin_w_label(VkCommandBuffer cmd, const char *label, float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);
    void WVK_API cmd_end_w_label(VkCommandBuffer cmd);

    template <typename T>
    void add_debug_w_label(VkDevice device, T handle, VkObjectType type, std::string_view name)
    {
        VkDebugUtilsObjectNameInfoEXT objectNameInfo = {};
        objectNameInfo.sType                         = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
        objectNameInfo.objectType                    = type;
        objectNameInfo.objectHandle                  = WVK_RCAST(uint64_t, handle);
        objectNameInfo.pObjectName                   = name.data();
        vk_assert(vkSetDebugUtilsObjectNameEXT(device, &objectNameInfo));
    }
#else
#	define add_debug_w_label(...) (void) 0
#	define cmd_begin_w_label(...) (void) 0
#	define cmd_end_w_label(...) (void) 0
#endif

#pragma endregion

#pragma region Image Utils
    struct CreateImageInfo {
        VkFormat format = VK_FORMAT_B8G8R8A8_SRGB;
        VkImageUsageFlags usage =
                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        VkImageCreateFlags flags = 0;
        VkExtent3D extents = {};
        uint32_t numLayers = 1;
        VkSampleCountFlags sampler = VK_SAMPLE_COUNT_1_BIT;
        VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
        bool mipMap = false;
        bool isCubeMap = false;
    };

    uint32_t bytes_per_pixel(VkFormat format);

    void WVK_API transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);

    void WVK_API
    blit_image(VkCommandBuffer cmd, VkImage src, VkImage dst, VkExtent2D src_extents, VkExtent2D dst_extents,
               VkFilter filter);

    void WVK_API generate_mipmaps();

    wvk::core::ImageData load_image(const std::filesystem::path &path);

#pragma endregion

#pragma region Rendering Utils

    struct RenderingInfo {
        VkRect2D renderArea{};
        VkImageView colorImageView = VK_NULL_HANDLE;
        VkImageView depthImageView = VK_NULL_HANDLE;
        VkImageView stencilImageView = VK_NULL_HANDLE;
        std::optional<float> depthImageClearValue;
        std::optional <VkClearValue> colorImageClearValue;
    };

    struct RenderInfo {
        VkRenderingAttachmentInfo colorAttachment = {};
        VkRenderingAttachmentInfo depthAttachment = {};
        VkRenderingAttachmentInfo stencilAttachment = {};
        VkRenderingInfo info = {};
    };

    RenderInfo WVK_API create_rendering_info(const RenderingInfo &renderingInfo);

    class Texture;

    void WVK_API clear(VkCommandBuffer cmd, const std::shared_ptr <Texture> &drawImage, const glm::vec4 &clearValue);

#pragma endregion

#pragma region Shader Utils

    VkShaderModule load_shader_module(VkDevice device, std::string_view filepath, std::string_view name = "");

#pragma endregion

#pragma region Pipeline Utils

    VkPipelineLayout create_pipeline_layout(VkDevice device, std::span<const VkDescriptorSetLayout> layouts,
                                            std::span<const VkPushConstantRange> pushConstantRanges,
                                            std::string_view name = "");

    class PipelineBuilder {
    public:
        explicit PipelineBuilder(VkPipelineLayout pipelineLayout);

        VkPipeline build(VkDevice device);

        PipelineBuilder &set_name(std::string_view name = "");

        PipelineBuilder &set_shaders(VkShaderModule vertexShader, VkShaderModule fragmentShader);

        PipelineBuilder &
        set_shaders(VkShaderModule vertexShader, VkShaderModule geometryShader, VkShaderModule fragmentShader);

        PipelineBuilder &set_vertex_input(std::span<const VkVertexInputBindingDescription> &bindings,
                                          std::span<const VkVertexInputAttributeDescription> &attributes);

        PipelineBuilder &set_input_topology(VkPrimitiveTopology topology);

        PipelineBuilder &set_polygon_mode(VkPolygonMode mode);

        PipelineBuilder &set_cull_mode(VkCullModeFlags cullMode, VkFrontFace frontFace);

        PipelineBuilder &enable_culling();

        PipelineBuilder &disable_culling();

        PipelineBuilder &set_multisampling_none();

        PipelineBuilder &set_multisampling(VkSampleCountFlagBits samples);

        PipelineBuilder &
        enable_blending(VkBlendOp blendOp = VK_BLEND_OP_ADD, VkBlendFactor srcBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
                        VkBlendFactor dstBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
                        VkBlendFactor srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
                        VkBlendFactor dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA);

        PipelineBuilder &disable_blending();

        PipelineBuilder &set_color_attachment_format(VkFormat format);

        PipelineBuilder &set_depth_format(VkFormat format);

        PipelineBuilder &enable_depth(bool depthWriteEnable, VkCompareOp op);

        PipelineBuilder &disable_depth_test();

        PipelineBuilder &enable_depth_clamp();

        PipelineBuilder &enable_dynamic_depth();

        PipelineBuilder &enable_depth_bias(float constantFactor, float slopeFactor);

    private:
        void clear();

        std::string _debug_name;

        std::vector <VkPipelineShaderStageCreateInfo> _shader_stages;
        std::span<const VkVertexInputBindingDescription> _vertex_bindings;
        std::span<const VkVertexInputAttributeDescription> _vertex_attributes;

        VkPipelineInputAssemblyStateCreateInfo _input_assembly = {};
        VkPipelineRasterizationStateCreateInfo _rasterization = {};
        VkPipelineColorBlendAttachmentState _color_attachment_blend = {};
        VkPipelineMultisampleStateCreateInfo _multisample = {};
        VkPipelineDepthStencilStateCreateInfo _depth_stencil = {};
        VkPipelineRenderingCreateInfo _render_info = {};
        VkFormat _color_attachment_format = VK_FORMAT_UNDEFINED;
        VkPipelineLayout _pipeline_layout = VK_NULL_HANDLE;
        bool _dynamic_depth = false;
    };

#pragma endregion
}        // namespace wvk::gfx