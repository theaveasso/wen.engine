#include "wen_renderer_be.hpp"

#include "vulkan/vk_renderer.hpp"

bool renderer_backend_init(WenRendererType type, WenRendererBackend *out_renderer_backend) {
    if (type == WenRendererType::VULKAN)
    {
        out_renderer_backend->init        = vk_renderer_init;
        out_renderer_backend->fini        = vk_renderer_fini;
        out_renderer_backend->resize      = vk_renderer_resize;
        out_renderer_backend->begin_frame = vk_renderer_begin_frame;
        out_renderer_backend->end_frame   = vk_renderer_end_frame;
    }
    return true;
}