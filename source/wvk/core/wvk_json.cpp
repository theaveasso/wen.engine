#include "wvk_json.hpp"

#include <utility>

namespace wvk::core
{
JsonFile::JsonFile(std::filesystem::path filepath) :
    _filepath(std::move(filepath))
{
}
JsonFile::JsonFile(nlohmann::json data) :
    _data(std::move(data))
{
}
JsonDataLoader::JsonDataLoader(const nlohmann::json &data) :
    _data(data),
    _override_mode(false)
{
}
JsonDataLoader::JsonDataLoader(const nlohmann::json &data, std::string_view name) :
    _data(data),
    _name(name),
    _override_mode(false)
{
}
JsonDataLoader JsonDataLoader::get_loader(std::string_view key) const
{
	auto it = _data.find(key);
	if (it != _data.end())
	{
		JsonDataLoader l(it.value(), _name + std::string(key));
		l.set_override(_override_mode);
		return l;
	}
	throw std::runtime_error("doesn't have a key");
}
JsonDataLoader JsonDataLoader::get_loader(size_t i) const
{
	if (i < _data.size())
	{
		JsonDataLoader l(_data[i], _name + "[" + std::to_string(i) + "]");
		l.set_override(_override_mode);
		return l;
	}
	throw std::runtime_error("doesn't have a key");
}
std::unordered_map<std::string, JsonDataLoader> JsonDataLoader::get_key_value_map() const
{
	std::unordered_map<std::string, JsonDataLoader> map(_data.size());
	for (const auto &[key, value] : _data.items())
	{
		JsonDataLoader loader(value, fmt::format("{}.{}", _name, key));
		loader.set_override(_override_mode);
		map.emplace(key, std::move(loader));
	}
	return map;
}
std::unordered_map<std::string, std::string> JsonDataLoader::get_key_value_map_string() const
{
	std::unordered_map<std::string, std::string> map(_data.size());
	for (const auto &[key, value] : _data.items())
	{
		map.emplace(key, value);
	}
	return map;
}
std::unordered_map<std::string, int> JsonDataLoader::get_key_value_map_int() const
{
	std::unordered_map<std::string, int> map(_data.size());
	for (const auto &[key, value] : _data.items())
	{
		map.emplace(key, value);
	}
	return map;
}
std::vector<JsonDataLoader> JsonDataLoader::get_vector() const
{
	std::vector<JsonDataLoader> outVector;
	outVector.reserve(_data.size());
	int i = 0;
	for (auto &value : _data)
	{
		JsonDataLoader loader(value, fmt::format("{}[{}]", _name, i));
		loader.set_override(_override_mode);
		outVector.push_back(std::move(loader));
		++i;
	}
	return outVector;
}
}        // namespace wvk::core
