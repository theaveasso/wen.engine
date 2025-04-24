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

#define WVK_MOVABLE_ONLY(CLASS_NAME)                         \
	CLASS_NAME(const CLASS_NAME &)                = delete;  \
	CLASS_NAME &operator=(const CLASS_NAME &)     = delete;  \
	CLASS_NAME(CLASS_NAME &&) noexcept            = default; \
	CLASS_NAME &operator=(CLASS_NAME &&) noexcept = default;

constexpr uint32_t CAMERA_SET               = 0;
constexpr uint32_t TEXTURES_AND_SAMPLER_SET = 1;
constexpr uint32_t VERTEX_INDEX_SET         = 2;
constexpr uint32_t MATERIAL_SET             = 3;
constexpr uint32_t BINDING_0                = 0;
constexpr uint32_t BINDING_1                = 1;
constexpr uint32_t BINDING_2                = 2;
constexpr uint32_t BINDING_3                = 3;

#define FRAME_OVERLAP 3

using std::make_shared;
using std::shared_ptr;
