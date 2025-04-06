#pragma once

#include <shaderc/shaderc.h>

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <array>
#include <vector>

#include <filesystem>
#include <optional>
#include <set>
#include <span>

#include <cassert>
#include <stdexcept>

#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <strstream>

#include "core/wen_logger.hpp"
#include "wen_private_defines.hpp"

#include "graphics/renderer/vulkan/vk_utils.hpp"
