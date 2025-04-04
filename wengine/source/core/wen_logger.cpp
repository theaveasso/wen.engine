#include "wen_logger.hpp"

static std::shared_ptr<spdlog::logger> logger = nullptr;

void log_init(log_level level, std::string_view pattern, bool enable_color) {
    if (enable_color) {
        logger = spdlog::stdout_color_mt("wen_logger");
    } else {
        logger = spdlog::stdout_color_mt("wen_logger");
    }

    logger->set_level(static_cast<spdlog::level::level_enum>(level));
    logger->set_pattern(pattern.data());
    spdlog::set_default_logger(logger);
}

void log_fini() {
 spdlog::shutdown();
 logger = nullptr;
}

spdlog::logger* log_get() {
    return logger.get();
}