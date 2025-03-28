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
#define WEN_API __attribute__((__visibility__("default")))
#elif defined(WEN_SYSTEM_WINDOWS)
#define WEN_API __declspec(dllexport)
#else
#error "System not support"
#endif
#else
#define WEN_API
#endif

#define KIB(VAL) ((VAL) * 1024)
#define MIB(VAL) ((VAL) * KIB(1024))
#define GIB(VAL) ((VAL) * MIB(1024))

#endif // WEN_DEFINES_HPP_
