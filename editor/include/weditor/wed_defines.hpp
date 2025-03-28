#ifndef WEDITOR_DEFINES_CPP_HPP_
#define WEDITOR_DEFINES_CPP_HPP_

#if defined(__APPLE__) && defined(__MACH__)
#define WEDITOR_SYSTEM_MACOS
#elif defined(_WIN32) || defined(_WIN64)
#define WEDITOR_SYSTEM_WINDOWS
#elif defined(__linux__)
#define WEN_SYSTEM_LINUX
#else
#error "Unsupported platform"
#endif

#if !defined(WEDITOR_STATIC)
#if defined(WEDITOR_SYSTEM_MACOS) || defined(WEDITOR_SYSTEM_LINUX)
#define WEN_API_EXPORT __attribute__((__visibility__("default")))
#define WEN_API_IMPORT __attribute__((__visibility__("default")))
#elif defined(WEDITOR_SYSTEM_WINDOWS)
#define WEDITOR_API_EXPORT __declspec(dllexport)
#define WEDITOR_API_IMPORT __declspec(dllimport)
#else
#error "System not support"
#endif
#else
#define WEDITOR_API_EXPORT
#define WEDITOR_API_IMPORT
#endif

#include <SDL3/SDL.h>
#include <memory>

using std::make_unique;
using std::unique_ptr;

using std::make_shared;
using std::shared_ptr;

#endif // WEDITOR_DEFINES_CPP_HPP_
