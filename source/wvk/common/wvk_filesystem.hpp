#pragma once

#include "wvk_pch.hpp"

namespace wvk::utils
{
std::vector<uint32_t> ReadShaderSpirv(std::string_view filename);
}
