/**
 * @file    wvk_logger.hpp
 * @author  theaveasso (theaveas.so@gmail.com)
 * @date    2025-04-13
 * @brief   Logging macro definitions for the WVK Engine using spdlog.
 *
 * This file defines a set of convenience macros for logging at various levels.
 * These macros use the engine's centralized spdlog logger to provide consistent and configurable output.
 */

#pragma once

#include "wvk/common/wvk_defines.hpp"
#include "wvk/common/wvk_pch.hpp"

#define WVK_LEVEL_TRACE 0

#define WVK_LEVEL_DEBUG 1

#define WVK_LEVEL_INFO 2

#define WVK_LEVEL_WARN 3

#define WVK_LEVEL_ERROR 4

#define WVK_LEVEL_FATAL 5

#define WVK_LEVEL_OFF 6

namespace wvk
{

enum struct LogLevel : int
{
	TRACE = WVK_LEVEL_TRACE,
	DEBUG = WVK_LEVEL_DEBUG,
	INFO  = WVK_LEVEL_INFO,
	WARN  = WVK_LEVEL_WARN,
	ERR   = WVK_LEVEL_ERROR,
	FATAL = WVK_LEVEL_FATAL,
	OFF   = WVK_LEVEL_OFF,
};

/**
 * @brief initialize engine logger using spdlog.
 * @param level the minimum log level to display.
 */
WVK_API void LoggerInit(LogLevel level);

/**
 * @brief de-initialize and cleans up the engine logger.
 */
WVK_API void LoggerFini();

/**
 * @brief retrieves the engine logger instance.
 */
WVK_API spdlog::logger *LoggerGet();

}        // namespace wvk

/** @brief Logs a trace-level message (very detailed, mostly for development). */
#define WVK_TRACE(...) \
	wvk::LoggerGet()->trace(__VA_ARGS__)

/** @brief Logs a debug-level message. */
#define WVK_DEBUG(...) \
	wvk::LoggerGet()->debug(__VA_ARGS__)

/** @brief Logs an info-level message. */
#define WVK_INFO(...) \
	wvk::LoggerGet()->info(__VA_ARGS__)

/** @brief Logs a warning-level message. */
#define WVK_WARN(...) \
	wvk::LoggerGet()->warn(__VA_ARGS__)

/** @brief Logs an error-level message. */
#define WVK_ERROR(...) \
	wvk::LoggerGet()->error(__VA_ARGS__)

/** @brief Logs a fatal/critical-level message. */
#define WVK_FATAL(...) \
	wvk::LoggerGet()->critical(__VA_ARGS__)
