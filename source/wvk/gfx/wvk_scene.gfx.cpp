#include "wvk_scene.gfx.hpp"

#include "wvk/core/wvk_gltf.hpp"

#include "wvk_gfx.hpp"
#include "wvk_inst.gfx.hpp"

static std::unordered_map<std::string, wvk::gfx::Scene> g_scenes;

namespace wvk::gfx
{

const Scene &scene_load(
    Instance                        &instance,
    const shared_ptr<MeshCache>     &meshCache,
    const shared_ptr<MaterialCache> &materialCache,
    const std::filesystem::path     &path)
{
	{
		const auto it = g_scenes.find(path.string());
		if (it != g_scenes.end())
		{
			return it->second;
		}
	}

	{
		Scene outScene;
		outScene.path = path;

		WVK_INFO("loading gtf scene '{}'\n", path.string());

		core::load_gltf_file(
		    instance,
		    meshCache,
		    materialCache,
		    path,
		    &outScene);

		auto [it, inserted] = g_scenes.emplace(path.string(), std::move(outScene));
		return it->second;
	}
}

const Scene &scene_get(
    std::string_view path)
{
	auto it = g_scenes.find(path.data());
	if (it == g_scenes.end())
	{
		throw std::runtime_error(fmt::format("scene '{}' was not loaded", path));
	}
	return it->second;
}

Scene &scene_mutable_get(
    std::string_view path)
{
	auto it = g_scenes.find(path.data());
	if (it == g_scenes.end())
	{
		throw std::runtime_error(fmt::format("scene '{}' was not loaded", path));
	}
	return it->second;
}
}        // namespace wvk::gfx
