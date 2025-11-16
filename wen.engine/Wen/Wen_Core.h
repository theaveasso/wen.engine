#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <string>
#include <typeindex>
#include <queue>
#include <stdexcept>
#include <algorithm>
#include <type_traits>
#include <functional>
#include <cstdint>
#include <unordered_map>

#define SPDLOG_USE_STD_FORMAT 1
#undef SPDLOG_FMT_EXTERNAL
#undef SPDLOG_COMPILED_LIB
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

template <typename T>
WEN_INLINE constexpr uint32_t Wen_TypeId() {
    return static_cast<uint32_t>(std::type_index(typeid(T)).hash_code());
}

#define WEN_DEFAULT_LOGGER_NAME "Wen"

struct WEN_API Wen_Logger {
    WEN_INLINE Wen_Logger() {
        auto console_sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
        console_sink->set_pattern("%^%T [WEN-%l]: [#%t] %v%$");

        std::vector<spdlog::sink_ptr> sinks = {console_sink};
        auto logger = std::make_shared<spdlog::logger>(WEN_DEFAULT_LOGGER_NAME, sinks.begin(), sinks.end());
        logger->set_level(spdlog::level::trace);
        logger->flush_on(spdlog::level::trace);
        spdlog::register_logger(logger);
    }

    WEN_INLINE ~Wen_Logger() {
        spdlog::shutdown();
    }
};


#ifdef WEN_ENABLE_LOGGING
#define WEN_TRACE(...)  if (auto logger = spdlog::get(WEN_DEFAULT_LOGGER_NAME)) { logger->trace(__VA_ARGS__); }
#define WEN_DEBUG(...)  if (auto logger = spdlog::get(WEN_DEFAULT_LOGGER_NAME)) { logger->debug(__VA_ARGS__); }
#define WEN_INFO(...)   if (auto logger = spdlog::get(WEN_DEFAULT_LOGGER_NAME)) { logger->info(__VA_ARGS__); }
#define WEN_WARN(...)   if (auto logger = spdlog::get(WEN_DEFAULT_LOGGER_NAME)) { logger->warn(__VA_ARGS__); }
#define WEN_ERROR(...)  if (auto logger = spdlog::get(WEN_DEFAULT_LOGGER_NAME)) { logger->error(__VA_ARGS__); }
#define WEN_FATAL(...)  if (auto logger = spdlog::get(WEN_DEFAULT_LOGGER_NAME)) { logger->critical(__VA_ARGS__); }
#else
#define WEN_TRACE(...)  (void)0
#define WEN_WARN(...)   (void)0
#define WEN_DEBUG(...)  (void)0
#define WEN_INFO(...)   (void)0
#define WEN_ERROR(...)  (void)0
#define WEN_FATAL(...)  (void)0
#endif
