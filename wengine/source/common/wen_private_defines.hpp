#pragma once

#if !defined(WEN_STATIC)
    #define WEN_API __declspec(dllexport)
#else
    #define WEN_API __declspec(dllimport)
#endif

#ifndef WEN_CAST
    #ifdef __cplusplus
        #define WEN_CAST(T, x) static_cast<T>(x)
    #else
        #define WEN_CAST(T, x) (T)(x)
    #endif
#endif
