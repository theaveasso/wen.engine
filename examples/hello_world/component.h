#pragma once

#include <wvk/wvk_wen.hpp>

using namespace wvk;

struct MeshComponent
{
	std::vector<MeshId>          meshes;
	std::vector<MaterialId>      meshMaterials;
	std::vector<core::Transform> meshTransforms;
	bool                         castShadow;
};

struct LightComponent
{
	gfx::Light light;
};