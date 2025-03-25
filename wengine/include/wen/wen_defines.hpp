#ifndef WEN_DEFINES_HPP_
#define WEN_DEFINES_HPP_

#if defined(__APPLE__) && defined(__MACH__)
#define WEN_SYSTEM_MACOS
#elif defined(_WIN32) || defined(_WIN64)
#define WEN_SYSTEM_WINDOWS
#elif defined(__linux__)
#define WEN_SYSTEM_LINUX
#else
#error "Unsupported platform"
#endif

#if !defined(WEN_STATIC)
#if defined(WEN_SYSTEM_MACOS) || defined(WEN_SYSTEM_LINUX)
#define WEN_API_EXPORT __attribute__((__visibility__("default")))
#define WEN_API_IMPORT __attribute__((__visibility__("default")))
#elif defined(WEN_SYSTEM_WINDOWS)
#define WEN_API_EXPORT __declspec(dllexport)
#define WEN_API_IMPORT __declspec(dllimport)
#else
#error "System not support"
#endif
#else
#define WEN_API_EXPORT
#define WEN_API_IMPORT
#endif

#include <SDL3/SDL.h>
#include <memory>

using std::make_unique;
using std::unique_ptr;

using std::make_shared;
using std::shared_ptr;

namespace wen {
using Color = SDL_Color;

} // namespace wen

#endif // WEN_DEFINES_HPP_
