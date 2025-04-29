#pragma once

#include "wvk/common/wvk_defines.hpp"

namespace wvk::core
{
class Transform;
}

namespace wvk::gfx
{

class Instance;
class MeshCache;
class MaterialCache;

struct Light;
struct CPUMesh;

struct SceneNode
{
	std::string name;        // glTF node name

	int meshIndex = -1;        // index into Scene::meshes
	int skinId    = -1;        // used for skeletal animation
	int lightId   = -1;        // if the node is a light
	int cameraId  = -1;        // if the node is a camera
};

struct SceneMesh
{
	std::vector<MeshId>     primitives;                // e.g. index into vertex/index buffer
	std::vector<MaterialId> primitiveMaterials;        // one material per primitive
};

struct Scene
{
	std::filesystem::path               path;
	std::vector<Light>                  lights;
	std::vector<SceneMesh>              meshes;
	std::unordered_map<MeshId, CPUMesh> cpuMeshes;
};

WVK_API const Scene &scene_load(
    Instance                        &instance,
    const shared_ptr<MeshCache>     &meshCache,
    const shared_ptr<MaterialCache> &materialCache,
    const std::filesystem::path     &path);

WVK_API const Scene &scene_get(
    std::string_view path);

WVK_API Scene &scene_mutable_get(
    std::string_view path);

}        // namespace wvk::gfx
