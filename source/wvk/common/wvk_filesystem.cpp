#include "wvk_filesystem.hpp"

namespace wvk::utils
{
std::vector<uint32_t> ReadShaderSpirv(std::string_view filename)
{
	std::string   path = (std::filesystem::current_path() / "etc/shaders" / std::string(std::string(filename) + ".spv")).string();
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
}        // namespace wvk::utils
