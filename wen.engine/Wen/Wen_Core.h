#pragma once

#include <memory>
#include <string>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#if defined (_WIN32) || defined(_WIN64)
#ifdef WEN_EXPORTS
#define WEN_API __declspec(dllexport)
#else
#define WEN_API __declspec(dllimport)
#endif
#else
#ifdef WEN_EXPORTS
#define WEN_API __attribute__((visibility("default")))
#else
#define WEN_API
#endif
#endif

#define WEN_ASSERT assert

#if defined (__clang__) || defined (__gcc__)
#define WEN_STATIC_ASSERT _Static_assert
#endif

#if defined (__clang__) || defined (__gcc__)
#define WEN_INLINE __attribute__((always_inline)) inline
#define WEN_NOINLINE __attribute__((noinline))
#endif

struct WEN_API Wen_Logger {
    WEN_INLINE Wen_Logger() {
        m_logger = spdlog::stdout_color_mt("stdout");
        spdlog::set_level(spdlog::level::trace);
        spdlog::set_pattern("%^[%T]: [#%t] %v%$");
    }

    WEN_INLINE static std::shared_ptr<spdlog::logger> GetLogger() {
        static Wen_Logger logger;
        return logger.m_logger;
    }

    private:
    std::shared_ptr<spdlog::logger> m_logger;
};

#ifdef WEN_ENABLE_LOGGING
#define WEN_TRACE(...) Wen_Logger::GetLogger()->trace(__VA_ARGS__)
#define WEN_WARN(...) Wen_Logger::GetLogger()->warn(__VA_ARGS__)
#define WEN_DEBUG(...) Wen_Logger::GetLogger()->debug(__VA_ARGS__)
#define WEN_INFO(...) Wen_Logger::GetLogger()->info(__VA_ARGS__)
#define WEN_ERROR(...) Wen_Logger::GetLogger()->error(__VA_ARGS__)
#define WEN_FATAL(...) Wen_Logger::GetLogger()->critical(__VA_ARGS__)
#else
#define WEN_TRACE(...)
#define WEN_WARN(...)
#define WEN_DEBUG(...)
#define WEN_INFO(...)
#define WEN_ERROR(...)
#define WEN_FATAL(...)
#endif
