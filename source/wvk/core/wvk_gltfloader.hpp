#pragma once

#include <tiny_gltf.h>

#include "wvk/common/wvk_pch.hpp"

namespace wvk::gfx
{

class MeshCache;

class MaterialCache;

struct Scene;

class Instance;
}        // namespace wvk::gfx

namespace wvk::core
{

gfx::Scene load_gltf_file(gfx::Instance &instance, gfx::MaterialCache &materialCache, const std::filesystem::path &path);
}        // namespace wvk::core