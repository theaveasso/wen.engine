#pragma once

#include "wen_core_options.hpp"

#include "privates/wen_private_defines.hpp"

#include "platform/wen_desktop_glfw.hpp"

#include "graphics/renderer/vulkan/vk_context.hpp"
#include "graphics/renderer/vulkan/vk_renderer.hpp"
#include "graphics/renderer/wen_renderer_ft.hpp"

enum struct WenEngineMode
{
	EDIT,
	RELEASE
};

enum struct WenEngineFeatures
{
	NONE           = 0,
	DEBUG_GRID     = 1 << 1,
	SKY_BOX        = 1 << 2,
	ENTITY_PICKING = 1 << 3,
};

struct WenEngine
{
	WenWindow         window;
	WenRenderer       renderer;
	WenEngineMode     mode     = {WenEngineMode::RELEASE};
	WenEngineFeatures features = {WenEngineFeatures::NONE};
};

WEN_API void engine_init(WenEngine *engine, WenEngineMode mode);
WEN_API void engine_fini(WenEngine *engine);
WEN_API bool engine_is_running(const WenEngine *engine);
WEN_API void engine_begin_frame();
WEN_API void engine_end_frame(WenEngine *engine);

WEN_API void engine_enable_features(WenEngine *engine, WenEngineFeatures features);
WEN_API void engine_disable_features(WenEngine *engine, WenEngineFeatures features);
