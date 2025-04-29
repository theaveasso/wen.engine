#include "wvk_gltf.hpp"

#include <tiny_gltf.h>

#include "wvk/gfx/wvk_gfx.hpp"
#include "wvk/gfx/wvk_inst.gfx.hpp"
#include "wvk/gfx/wvk_scene.gfx.hpp"

namespace
{
const std::string GLTF_POSITIONS_ACCESSOR{"POSITION"};
const std::string GLTF_NORMALS_ACCESSOR{"NORMAL"};
const std::string GLTF_TANGENTS_ACCESSOR{"TANGENT"};
const std::string GLTF_UVS_ACCESSOR{"TEXCOORD_0"};
// const std::string GLTF_JOINTS_ACCESSOR{"JOINTS_0"};
// const std::string GLTF_WEIGHTS_ACCESSOR{"WEIGHTS_0"};

// const std::string GLTF_SAMPLER_PATH_TRANSLATION{"translation"};
// const std::string GLTF_SAMPLER_PATH_ROTATION{"rotation"};
// const std::string GLTF_SAMPLER_PATH_SCALE{"scale"};

// extensions
// const std::string GLTF_LIGHTS_PUNCTUAL_KEY{"KHR_lights_punctual"};
const std::string GLTF_LIGHTS_PUNCTUAL_POINT_NAME{"point"};
const std::string GLTF_LIGHTS_PUNCTUAL_DIRECTIONAL_NAME{"directional"};
const std::string GLTF_LIGHTS_PUNCTUAL_SPOT_NAME{"spot"};

const std::string GLTF_EMISSIVE_STRENGTH_KHR_KEY{"KHR_materials_emissive_strength"};
const std::string GLTF_EMISSIVE_STRENGTH_PARAM_NAME{"emissiveStrength"};

glm::vec2 tg_to_vec2(
    const std::vector<double> &vec)
{
	assert(vec.size() == 2);
	return {vec[0], vec[1]};
}

glm::vec3 tg_to_vec3(
    const std::vector<double> &vec)
{
	assert(vec.size() == 3);
	return {vec[0], vec[1], vec[2]};
}

glm::vec4 tg_to_vec4(
    const std::vector<double> &vec)
{
	if (vec.size() == 4)
	{
		return {vec[0], vec[1], vec[2], vec[3]};
	}

	assert(vec.size() == 3);
	return {vec[0], vec[1], vec[2], 1.f};
}

glm::quat tg_to_quat(const std::vector<double> &vec)
{
	return {WVK_CAST(float, vec[3]), WVK_CAST(float, vec[0]), WVK_CAST(float, vec[1]), WVK_CAST(float, vec[2])};
}

wvk::ColorRGBA tg_to_color_rgba(
    const std::vector<double> &vec)
{
	if (vec.size() == 4)
	{
		return {WVK_CAST(float, vec[0]), WVK_CAST(float, vec[1]), WVK_CAST(float, vec[2]), WVK_CAST(float, vec[3])};
	}
	return {WVK_CAST(float, vec[0]), WVK_CAST(float, vec[1]), WVK_CAST(float, vec[2]), 1};
}

int32_t find_attri_accessor(
    const tinygltf::Primitive &priv,
    const std::string         &attriName)
{
	for (const auto &[name, id] : priv.attributes)
	{
		if (name == attriName)
		{
			return id;
		}
	}
	return -1;
}

template <typename T>
std::span<const T> get_packed_buffer_span(
    const tinygltf::Model    &model,
    const tinygltf::Accessor &accessor)
{
	const auto &bv = model.bufferViews[accessor.bufferView];
	const int   bs = accessor.ByteStride(bv);

	const int componentSize  = tinygltf::GetComponentSizeInBytes(accessor.componentType);
	const int componentCount = tinygltf::GetNumComponentsInType(accessor.type);
	const int expectedSize   = componentSize * componentCount;

	WVK_ASSERT_MSG(bs == 0 || bs == expectedSize, "byte stride mismatch");

	const auto &buffer = model.buffers[bv.buffer];
	const auto *data   = WVK_RCAST(const T *, &buffer.data.at(0) + bv.byteOffset + accessor.byteOffset);
	return {data, accessor.count};
}

template <typename T>
std::span<const T> get_packed_buffer_span(
    const tinygltf::Model     &model,
    const tinygltf::Primitive &priv,
    const std::string         &attriName)
{
	const auto index = find_attri_accessor(priv, attriName);
	WVK_ASSERT_MSG(index != -1, "accessor not found");
	const auto &accessor = model.accessors[index];
	return get_packed_buffer_span<T>(model, accessor);
}

bool has_accessor(
    const tinygltf::Primitive &priv,
    const std::string         &attriName)
{
	const auto id = find_attri_accessor(priv, attriName);
	return id != -1;
}

bool has_diffuse_texture(
    const tinygltf::Material &material)
{
	return material.pbrMetallicRoughness.baseColorTexture.index != -1;
}

bool has_normal_map_texture(
    const tinygltf::Material &material)
{
	return material.normalTexture.index != -1;
}

bool has_metallic_roughness_texture(
    const tinygltf::Material &material)
{
	return material.pbrMetallicRoughness.metallicRoughnessTexture.index != -1;
}

bool has_emissive_texture(
    const tinygltf::Material &material)
{
	return material.emissiveTexture.index != -1;
}

wvk::ColorRGBA get_diffuse_color(
    const tinygltf::Material &material)
{
	const auto color = material.pbrMetallicRoughness.baseColorFactor;
	WVK_ASSERT_MSG(color.size() == 4, "invalid diffuse color");
	return tg_to_color_rgba(color);
}

std::filesystem::path get_diffuse_texture_path(
    const tinygltf::Model       &model,
    const tinygltf::Material    &material,
    const std::filesystem::path &path)
{
	const auto  textureIndex = material.pbrMetallicRoughness.baseColorTexture.index;
	const auto  textureId    = model.textures[textureIndex];
	const auto &image        = model.images[textureId.source];
	return path / image.uri;
}

std::filesystem::path get_normals_texture_path(
    const tinygltf::Model       &model,
    const tinygltf::Material    &material,
    const std::filesystem::path &path)
{
	const auto  textureIndex = material.normalTexture.index;
	const auto  textureId    = model.textures[textureIndex];
	const auto &image        = model.images[textureId.source];
	return path / image.uri;
}

std::filesystem::path get_metallic_roughness_texture_path(
    const tinygltf::Model       &model,
    const tinygltf::Material    &material,
    const std::filesystem::path &path)
{
	const auto  textureIndex = material.pbrMetallicRoughness.metallicRoughnessTexture.index;
	const auto  textureId    = model.textures[textureIndex];
	const auto &image        = model.images[textureId.source];
	return path / image.uri;
}

std::filesystem::path get_emissive_texture_path(
    const tinygltf::Model       &model,
    const tinygltf::Material    &material,
    const std::filesystem::path &path)
{
	const auto  textureIndex = material.emissiveTexture.index;
	const auto  textureId    = model.textures[textureIndex];
	const auto &image        = model.images[textureId.source];
	return path / image.uri;
}

float get_emissive_strength(
    const tinygltf::Material &material)
{
	if (material.extensions.contains(GLTF_EMISSIVE_STRENGTH_KHR_KEY))
	{
		return WVK_CAST(
		    float,
		    material.extensions.at(GLTF_EMISSIVE_STRENGTH_KHR_KEY)
		        .Get(GLTF_EMISSIVE_STRENGTH_PARAM_NAME)
		        .GetNumberAsDouble());
	}
	return 1.f;
}

bool LoadImageData(
    tinygltf::Image * /* image */,
    const int /* imageIdx */,
    std::string * /* err */,
    std::string * /* warn */,
    int /* reqWidth */,
    int /* reqHeight */,
    const unsigned char * /* bytes */,
    int /* size */,
    void *)
{
	return true;
}

bool WriteImageData(
    const std::string * /* basepath */,
    const std::string * /* filename */,
    const tinygltf::Image * /* image */,
    bool /* embedImages */,
    const tinygltf::FsCallbacks * /* fs_cb */,
    const tinygltf::URICallbacks * /* uri_cb */,
    std::string * /* out_uri */,
    void * /* user_pointer */)
{
	return true;
}

void set_empty_image_loader(
    tinygltf::TinyGLTF &loader)
{
	loader.SetImageLoader(LoadImageData, nullptr);
	loader.SetImageWriter(WriteImageData, nullptr);
}

void load_gltf_file(
    tinygltf::Model             &gltfModel,
    const std::filesystem::path &path)
{
	tinygltf::TinyGLTF loader;
	set_empty_image_loader(loader);

	std::string err;
	std::string warn;

	bool res = loader.LoadASCIIFromFile(
	    &gltfModel,
	    &err,
	    &warn,
	    path.string());
	if (!warn.empty())
	{
		WVK_WARN(warn);
	}
	if (!res)
	{
		if (!err.empty())
		{
			WVK_ERROR(err);
		}
		WVK_ASSERT_MSG(false, "failed to load gltf scene");
	}
}

wvk::gfx::CPUMesh tg_to_cpu_mesh(
    const tinygltf::Model     &model,
    const tinygltf::Primitive &primitive,
    const std::string         &name)
{
	wvk::gfx::CPUMesh outMesh = {};
	outMesh.name              = name;

	// load indices
	if (primitive.indices != 1)
	{
		const auto &indexAccessor = model.accessors[primitive.indices];
		const auto  indices       = get_packed_buffer_span<uint16_t>(model, indexAccessor);
		outMesh.indices.assign(indices.begin(), indices.end());
	}

	// load positions
	const auto positions = get_packed_buffer_span<glm::vec3>(model, primitive, GLTF_POSITIONS_ACCESSOR);
	outMesh.vertices.resize(positions.size());
	for (size_t i = 0; i < positions.size(); ++i)
	{
		outMesh.vertices[i].position = positions[i];
	}

	// get min and max pos
	const auto posAccessId = find_attri_accessor(primitive, GLTF_POSITIONS_ACCESSOR);
	WVK_ASSERT_MSG(posAccessId != -1, "accessor not found");
	const auto &posAccessor = model.accessors[posAccessId];
	outMesh.minPos          = tg_to_vec3(posAccessor.minValues);
	outMesh.maxPos          = tg_to_vec3(posAccessor.maxValues);

	const auto numVertices = positions.size();

	// load normals
	if (has_accessor(primitive, GLTF_NORMALS_ACCESSOR))
	{
		const auto &normals = get_packed_buffer_span<glm::vec3>(model, primitive, GLTF_NORMALS_ACCESSOR);
		WVK_ASSERT_MSG(normals.size() == numVertices, "invalid normals size");
		for (size_t i = 0; i < normals.size(); ++i)
		{
			outMesh.vertices[i].normal = normals[i];
		}
	}

	// load tangents
	if (has_accessor(primitive, GLTF_TANGENTS_ACCESSOR))
	{
		const auto &tangents = get_packed_buffer_span<glm::vec4>(model, primitive, GLTF_TANGENTS_ACCESSOR);
		WVK_ASSERT_MSG(tangents.size() == numVertices, "invalid tangents size");
		for (size_t i = 0; i < tangents.size(); ++i)
		{
			outMesh.vertices[i].tangent = tangents[i];
		}
	}

	// load uvs
	if (has_accessor(primitive, GLTF_UVS_ACCESSOR))
	{
		const auto uvs = get_packed_buffer_span<glm::vec2>(model, primitive, GLTF_UVS_ACCESSOR);
		WVK_ASSERT_MSG(uvs.size() == numVertices, "invalid uvs size");
		for (size_t i = 0; i < uvs.size(); ++i)
		{
			outMesh.vertices[i].uv_x = uvs[i].x;
			outMesh.vertices[i].uv_y = uvs[i].y;
		}
	}

	return outMesh;
}

wvk::gfx::Material tg_to_material(
    wvk::gfx::Instance          &instance,
    const tinygltf::Model       &model,
    const tinygltf::Material    &material,
    const std::filesystem::path &path)
{
	wvk::gfx::Material outMaterial = {};
	outMaterial.name               = material.name;
	outMaterial.baseColor          = get_diffuse_color(material);
	outMaterial.roughnessFactor    = WVK_CAST(float, material.pbrMetallicRoughness.roughnessFactor);
	outMaterial.metallicFactor     = WVK_CAST(float, material.pbrMetallicRoughness.metallicFactor);

	if (has_diffuse_texture(material))
	{
		const auto diffusePath     = get_diffuse_texture_path(model, material, path);
		outMaterial.diffuseTexture = instance.make_texture_w_path(
		    VK_FORMAT_R8G8B8A8_SRGB,
		    VK_IMAGE_USAGE_SAMPLED_BIT,
		    diffusePath,
		    "diffuse_" + material.name);
	}

	if (has_normal_map_texture(material))
	{
		const auto normalsPath       = get_normals_texture_path(model, material, path);
		outMaterial.normalMapTexture = instance.make_texture_w_path(
		    VK_FORMAT_R8G8B8A8_UNORM,
		    VK_IMAGE_USAGE_SAMPLED_BIT,
		    normalsPath,
		    "normals_" + material.name);
	}

	if (has_emissive_texture(material))
	{
		const auto emissivePath     = get_emissive_texture_path(model, material, path);
		outMaterial.emissiveTexture = instance.make_texture_w_path(
		    VK_FORMAT_R8G8B8A8_SRGB,
		    VK_IMAGE_USAGE_SAMPLED_BIT,
		    emissivePath,
		    "emissive_" + material.name);
		outMaterial.emissiveFactor = get_emissive_strength(material);
	}

	if (has_metallic_roughness_texture(material))
	{
		const auto metallicRoughnessPath     = get_metallic_roughness_texture_path(model, material, path);
		outMaterial.metallicRoughnessTexture = instance.make_texture_w_path(
		    VK_FORMAT_R8G8B8A8_UNORM,
		    VK_IMAGE_USAGE_SAMPLED_BIT,
		    metallicRoughnessPath,
		    "metallic_roughness_" + material.name);
	}

	return outMaterial;
}

wvk::gfx::LightType tg_to_light_type(
    const std::string &lightType)
{
	if (lightType == GLTF_LIGHTS_PUNCTUAL_POINT_NAME)
	{
		return wvk::gfx::LightType::POINT;
	}
	else if (lightType == GLTF_LIGHTS_PUNCTUAL_SPOT_NAME)
	{
		return wvk::gfx::LightType::SPOT;
	}
	else if (lightType == GLTF_LIGHTS_PUNCTUAL_DIRECTIONAL_NAME)
	{
		return wvk::gfx::LightType::DIRECTIONAL;
	}
	WVK_WARN("unexpected light type {}", lightType);
	WVK_ASSERT_MSG(false, "unexpected light type");
	return wvk::gfx::LightType::NONE;
}

float tg_to_light_intensity(double intensity)
{
	const auto PBR_WATTS_TO_LUMENS = 683.f;
	return WVK_CAST(float, intensity / PBR_WATTS_TO_LUMENS);
}

wvk::gfx::Light tg_to_light(
    const tinygltf::Light &light)
{
	wvk::gfx::Light outLight = {};
	outLight.name            = light.name;
	outLight.type            = tg_to_light_type(light.type);
	outLight.color           = tg_to_color_rgba(light.color);
	outLight.range           = WVK_CAST(float, light.range);
	outLight.intensity       = tg_to_light_intensity(light.intensity);
	wvk::gfx::set_cone_angles(WVK_CAST(float, light.spot.innerConeAngle), WVK_CAST(float, light.spot.outerConeAngle), &outLight);

	if (outLight.type == wvk::gfx::LightType::POINT)
	{
		outLight.intensity /= 1.75f;
	}
	return outLight;
}
}        // namespace

namespace wvk::core
{

void load_gltf_file(
    gfx::Instance                        &instance,
    const shared_ptr<gfx::MeshCache>     &meshCache,
    const shared_ptr<gfx::MaterialCache> &materialCache,
    const std::filesystem::path          &path,
    gfx::Scene                           *outScene)
{
	outScene->path = path;

	const auto fileDir = path.parent_path();

	tinygltf::Model gltfModel;
	::load_gltf_file(gltfModel, path);

	const auto gltfScene = gltfModel.scenes[gltfModel.defaultScene];

	// load materials
	std::unordered_map<size_t, MaterialId> materialMapping;
	for (size_t matId = 0; matId < gltfModel.materials.size(); ++matId)
	{
		const auto &gltfMaterial = gltfModel.materials[matId];
		const auto  materialId   = materialCache->add_material(
            instance,
            tg_to_material(instance, gltfModel, gltfMaterial, fileDir));
		materialMapping.emplace(matId, materialId);
	}

	// load meshes
	outScene->meshes.reserve(gltfModel.meshes.size());
	for (const auto &gltfMesh : gltfModel.meshes)
	{
		gfx::SceneMesh mesh;
		mesh.primitives.resize(gltfMesh.primitives.size());
		mesh.primitiveMaterials.resize(gltfMesh.primitives.size());

		for (size_t privId = 0; privId < gltfMesh.primitives.size(); ++privId)
		{
			// load to cpu
			const auto &gltfPriv = gltfMesh.primitives[privId];
			auto        cpuMesh  = tg_to_cpu_mesh(gltfModel, gltfPriv, gltfMesh.name);
			if (cpuMesh.indices.empty())
			{
				continue;
			}

			// upload to gpu
			auto materialId = materialCache->get_placeholder_material_id();
			if (gltfPriv.material != -1)
			{
				materialId = materialMapping.at(gltfPriv.material);
			}
			const auto meshId = meshCache->add_mesh(
			    instance,
			    cpuMesh);
			mesh.primitives[privId]         = meshId;
			mesh.primitiveMaterials[privId] = materialId;
			outScene->cpuMeshes.emplace(meshId, std::move(cpuMesh));
		}
		outScene->meshes.push_back(std::move(mesh));
	}

	// load lights
	outScene->lights.resize(gltfModel.lights.size());
	for (const auto &light : gltfModel.lights)
	{
		outScene->lights.push_back(tg_to_light(light));
	}
}
}        // namespace wvk::core
