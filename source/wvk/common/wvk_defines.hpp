#pragma once

#include "wvk_logger.hpp"

/**
 * @brief Helper macro to perform a static_cast in a consistent, readable way.
 * @param T The target type.
 * @param V The value to be cast.
 */
#ifndef WVK_CAST
#	define WVK_CAST(T, V) static_cast<T>((V))
#endif

#ifndef WVK_RCAST
#	define WVK_RCAST(T, V) reinterpret_cast<T>((V))
#endif

#ifndef WVK_MIN
#	define WVK_MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef WVK_MAX
#	define WVK_MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

/**
 * @def WVK_API
 * @brief Platform-specific macro to handle symbol export/import for dynamic linking.
 */
#if defined(_WIN32) || defined(_WIN64)
#	if !defined(WVkEngine_EXPORTS)
#		define WVK_API __declspec(dllimport)
#	else
#		define WVK_API __declspec(dllexport)
#	endif
#else
#	define WVK_API
#endif

// clang-format off
#if !defined(NDEBUG) && (defined(DEBUG) || defined(_DEBUG) || defined(__DEBUG))
	#define WVK_DBG

	#define WVK_ASSERT_MSG(expr, ...) do { 							\
 		if (!(expr)) {                   							\
			wvk::LoggerGet()->error(__VA_ARGS__);					\
     		assert(false);                                       	\
		}                                         					\
	} while (0)
#else
	#define WVK_NDBG
#endif
// clang-format on

#if defined(TRACY_ENABLE)
// clang-format off
#include <tracy/Tracy.hpp>

#define WVK_PROFILER_COLOR_WAIT 		0xff0000
#define WVK_PROFILER_COLOR_SUBMIT 		0x0000ff
#define WVK_PROFILER_COLOR_PRESENT 		0x00ff00
#define WVK_PROFILER_COLOR_CREATE 		0xff6600
#define WVK_PROFILER_COLOR_DESTROY 		0xffa500
#define WVK_PROFILER_COLOR_BARRIER 		0xffffff
#define WVK_PROFILER_COLOR_CMD_DRAW 	tracy::Color::VioletRed
#define WVK_PROFILER_COLOR_CMD_COPY 	0x8b0a50
#define WVK_PROFILER_COLOR_CMD_RTX 		0x8b0aff
#define WVK_PROFILER_COLOR_CMD_DISPATCH 0x8b0aff

#define WVK_PROFILER_FUNC() ZoneScoped
#define WVK_PROFILER_FUNC_COLOR(color) ZoneScopedC(color)
#define WVK_PROFILER_ZONE(name, color)     \
		{                                  \
			ZoneScopedC(color);            \
			ZoneName(name, strlen(name))
#define WVK_PROFILER_ZONE_END() }
#define WVK_PROFILER_THREAD(name) tracy::SetThreadName(name)
#define WVK_PROFILER_FRAME(name) FrameMarkNamed(name)
#else
#define WVK_PROFILER_FUNC() 			(void) 0
#define WVK_PROFILER_FUNC_COLOR(color)  (void) 0
#define WVK_PROFILER_ZONE(name, color)	(void) 0
#define WVK_PROFILER_ZONE_END() 		(void) 0
#define WVK_PROFILER_THREAD(name) 		(void) 0
#define WVK_PROFILER_FRAME(name) 		(void) 0
// clang-format on
#endif

#define WVK_MOVABLE_ONLY(CLASS_NAME)                         \
	CLASS_NAME(const CLASS_NAME &)                = delete;  \
	CLASS_NAME &operator=(const CLASS_NAME &)     = delete;  \
	CLASS_NAME(CLASS_NAME &&) noexcept            = default; \
	CLASS_NAME &operator=(CLASS_NAME &&) noexcept = default;

#define FRAME_OVERLAP 3

using std::make_shared;
using std::shared_ptr;

namespace wvk
{
using TextureId                         = uint32_t;
static constexpr size_t NULL_TEXTURE_ID = std::numeric_limits<uint32_t>::max();

using MeshId                         = size_t;
static constexpr size_t NULL_MESH_ID = std::numeric_limits<size_t>::max();

using MaterialId                         = uint32_t;
static constexpr size_t NULL_MATERIAL_ID = std::numeric_limits<uint32_t>::max();

}        // namespace wvk
