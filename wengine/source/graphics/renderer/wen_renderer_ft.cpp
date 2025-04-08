#include "wen_renderer_ft.hpp"

#include "wen_renderer_be.hpp"

#include <cassert>

static WenRendererBackend *backend;

void renderer_init(
    WenRendererType type,
    WenRenderer    *renderer,
    WenWindow      &window,
    const char     *app_name,
    int32_t         width,
    int32_t         height)
{
	assert(renderer != nullptr);
	renderer->backend = (WenRendererBackend *) malloc(sizeof(WenRendererBackend));
	if (!renderer->backend)
	{
		EXIT_FAILURE;
	}
	renderer_backend_init(type, renderer->backend);

	if (!renderer->backend->init(renderer->backend, window, app_name, width, height))
	{
		exit(EXIT_FAILURE);
	}
}

void renderer_fini(WenRenderer *renderer)
{
	if (renderer->backend)
	{
		renderer->backend->fini(renderer->backend);
		renderer->backend = nullptr;
	}
	WEN_DEBUG("[Renderer] successfully destroyed!");
}

void renderer_resize(WenRenderer *renderer, uint32_t width, uint32_t height)
{
	if (renderer->backend)
	{
		renderer->backend->resize(renderer->backend, width, height);
	}
}

bool renderer_begin_frame(WenRenderer *renderer)
{
	if (renderer->backend)
	{
		return renderer->backend->begin_frame(renderer->backend, 0);
	}
	return true;
}

bool renderer_end_frame(WenRenderer *renderer)
{
	if (renderer->backend)
	{
		return renderer->backend->end_frame(renderer->backend, 0);
	}
	return true;
}

void renderer_draw_frame(WenRenderer *renderer)
{
	if (!renderer_begin_frame(renderer))
	{
		WEN_WARN("[Renderer] failed renderer begin frame!");
		return;
	};
	if (!renderer_end_frame(renderer))
	{
		WEN_FATAL("[Renderer] failed renderer end frame!");
		abort();
	};
}
