#pragma once

#include "privates/wen_pch.hpp"

#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#define WEN_LEVEL_TRACE 0
#define WEN_LEVEL_DEBUG 1
#define WEN_LEVEL_INFO 2
#define WEN_LEVEL_WARN 3
#define WEN_LEVEL_ERROR 4
#define WEN_LEVEL_CRITICAL 5
#define WEN_LEVEL_OFF 6

enum struct log_level : int
{
	trace    = WEN_LEVEL_TRACE,
	debug    = WEN_LEVEL_DEBUG,
	info     = WEN_LEVEL_INFO,
	warn     = WEN_LEVEL_WARN,
	err      = WEN_LEVEL_ERROR,
	critical = WEN_LEVEL_CRITICAL,
	off      = WEN_LEVEL_OFF,
};

void log_init(
    log_level        level        = log_level::info,
    std::string_view pattern      = "[%H:%M:%S %z] %l: %^%v%$",
    bool             enable_color = true);

void log_fini();

spdlog::logger *
    log_get();

#define WEN_TRACE(...) log_get()->trace(__VA_ARGS__)
#define WEN_DEBUG(...) log_get()->debug(__VA_ARGS__)
#define WEN_INFO(...) log_get()->info(__VA_ARGS__)
#define WEN_WARN(...) log_get()->warn(__VA_ARGS__)
#define WEN_ERROR(...) log_get()->error(__VA_ARGS__)
#define WEN_FATAL(...) log_get()->critical(__VA_ARGS__)

#define WEN_ASSERT(expr, ...)                        \
	if (!(expr))                                     \
	{                                                \
		WEN_ERROR("Assertion failed: " __VA_ARGS__); \
		assert(expr);                                \
	}

#define WEN_ABORT(expr, ...)                         \
	if (!(expr))                                     \
	{                                                \
		WEN_ERROR("Assertion failed: " __VA_ARGS__); \
		abort();                                     \
	}
