#include "wen_filesystem.hpp"

std::vector<uint32_t> wen_read_shader_spirv(std::string &filename)
{
	auto          path = (std::filesystem::current_path() / "shaders" / std::string(filename + ".glsl.spv")).string();
	std::ifstream file(path, std::ios::binary | std::ios::ate);
	if (!file.is_open())
	{
		throw std::runtime_error("failed to open SPIR-V file " + path);
	}

	std::streamsize size = file.tellg();
	if (size % sizeof(uint32_t) != 0)
	{
		throw std::runtime_error("SPIR-V file is not aligned to uin32_t " + path);
	}

	file.seekg(0, std::ios::beg);
	std::vector<uint32_t> buffer(size / sizeof(uint32_t));
	file.read(reinterpret_cast<char *>(buffer.data()), size);

	if (!file)
	{
		throw std::runtime_error("failed to read SPIR-V file completely " + path);
	}

	return buffer;
}