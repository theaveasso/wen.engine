#include "wvk_logger.hpp"

namespace wvk
{
static std::shared_ptr<spdlog::logger> g_logger = nullptr;

void LoggerInit(LogLevel level)
{
	assert(g_logger == nullptr);

	g_logger = spdlog::stdout_color_mt("WENVK");
	g_logger->set_level(WVK_CAST(spdlog::level::level_enum, level));
	spdlog::set_default_logger(g_logger);
}

void LoggerFini()
{
	spdlog::shutdown();
	g_logger = nullptr;
}

spdlog::logger *LoggerGet()
{
	return g_logger.get();
}
}        // namespace wvk
