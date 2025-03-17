#pragma once

#if defined(__APPLE__) && defined(__MACH__)
#    define WEN_SYSTEM_MACOS
#elif defined(__WIN32) || defined(__WIN64)
#    define WEN_SYSTEM_WINDOWS
#elif defined(__linux__)
#    define WEN_SYSTEM_LINUX
#else
#    error "Unsupported platform"
#endif

#if !defined(WEN_STATIC)
#    if defined(WEN_SYSTEM_MACOS) || defined(WEN_SYSTEM_LINUX)
#        define WEN_API_EXPORT __attribute__((__visibility__("default")))
#        define WEN_API_IMPORT __attribute__((__visibility__("default")))
#    elif defined(WEN_SYSTEM_WINDOWS)
#        define WEN_API_EXPORT __declspec(dllexport)
#        define WEN_API_IMPORT __declspec(dllimport)
#        error "System not support"
#    else
#        define WEN_API_EXPORT
#        define WEN_API_IMPORT
#    endif

#endif // __WE_DEFINES_HPP_
