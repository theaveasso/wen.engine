#pragma once

#include "wen_logger.hpp"

enum struct WenRendererType
{
  VULKAN,
  OPENGL,
  DIRECTX12
};

namespace core_opts
{
    inline constexpr int             start_width  = 1920;
    inline constexpr int             start_height = 1080;
    inline constexpr WenRendererType renderer     = WenRendererType::VULKAN;
    inline log_level                 level        = log_level::info;
}