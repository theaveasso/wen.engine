#pragma once

#include "wvk/common/wvk_pch.hpp"

namespace wvk::gfx
{

class MeshCache;
class MaterialCache;
class Instance;

struct Scene;

}        // namespace wvk::gfx

namespace wvk::core
{

WVK_API void load_gltf_file(
    gfx::Instance                 &instance,
    const shared_ptr<gfx::MeshCache>&     meshCache,
    const shared_ptr<gfx::MaterialCache>& materialCache,
    const std::filesystem::path   &path,
    gfx::Scene                    *outScene);

}        // namespace wvk::core