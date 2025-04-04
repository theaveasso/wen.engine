#pragma once

#if !defined(WEN_STATIC)
    #define WEN_API __declspec(dllexport)
#else
    #define WEN_API __declspec(dllimport)
#endif
