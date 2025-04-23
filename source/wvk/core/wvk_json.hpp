#pragma once

#include "wvk/common/wvk_pch.hpp"
#include "wvk/common/wvk_defines.hpp"
#include <nlohmann/json.hpp>

namespace nlohmann
{
template <>
struct adl_serializer<std::filesystem::path>
{
	static void to_json(nlohmann::json &j, const std::filesystem::path &filepath) { j = filepath.string(); }
	static void from_json(const nlohmann::json &j, std::filesystem::path &filepath) { filepath = std::filesystem::path(j.get<std::string>()); }
};

}        // namespace nlohmann
namespace wvk::core
{
class WVK_API JsonFile
{
  public:
	JsonFile() = default;
	explicit JsonFile(std::filesystem::path filepath);
	explicit JsonFile(nlohmann::json data);

	[[nodiscard]] inline bool is_good() const { return _is_good; }

  private:
	bool                  _is_good       = true;
	bool                  _override_mode = true;
	nlohmann::json        _data;
	std::filesystem::path _filepath;
};

class WVK_API JsonDataLoader
{
  public:
	explicit JsonDataLoader(const nlohmann::json &data);
	JsonDataLoader(const nlohmann::json &data, std::string_view name);

	inline void               set_override(bool b) { _override_mode = b; }
	[[nodiscard]] inline bool is_override() const { return _override_mode; }

	[[nodiscard]] JsonDataLoader get_loader(std::string_view key) const;
	[[nodiscard]] JsonDataLoader get_loader(size_t i) const;

	[[nodiscard]] inline const std::string &get_name() const { return _name; }

	template <typename T>
	void get(std::string_view valueName, T &obj) const
	{
		get_impl(valueName, obj);
	}

	template <typename T>
	void get(std::string_view valueName, T &obj, const T &defaultValue) const
	{
		get_impl(valueName, obj, &defaultValue);
	}

	template <typename T>
	void get_if_exists(std::string_view valueName, T &obj) const
	{
		if (!has_key(valueName))
		{
			return;
		}
		get(valueName, obj);
	}

	template <typename T>
	[[nodiscard]] bool has_key(const T &key) const
	{
		return _data.find(key) != _data.end();
	}

	template <typename T>
	std::vector<T> as_vector_of() const
	{
		std::vector<T> outVector;
		outVector.reserve(_data.size());
		for (const auto &value : _data)
		{
			outVector.push_back(value);
		}
		return outVector;
	}

	[[nodiscard]] inline const nlohmann::json &get_json() const { return _data; }
	[[nodiscard]] size_t                       get_size() const { return _data.size(); }

	[[nodiscard]] std::unordered_map<std::string, JsonDataLoader> get_key_value_map() const;
	[[nodiscard]] std::unordered_map<std::string, std::string>    get_key_value_map_string() const;
	[[nodiscard]] std::unordered_map<std::string, int>            get_key_value_map_int() const;
	[[nodiscard]] std::vector<JsonDataLoader>                     get_vector() const;

  private:
	template <typename T>
	void get_impl(std::string_view valueName, T &obj, const T *defaultValuePtr = nullptr) const
	{
		WVK_ASSERT_MSG(!std::is_enum_v<T>, "use getEnumValue instead!");
		if (const auto it = _data.find(valueName); it != _data.end())
		{
			try
			{
				obj = it.value().get<T>();
			}
			catch (const std::exception &e)
			{
				throw std::runtime_error(e.what());
			}
		}
		else
		{
			if (_override_mode)
			{
				return;
			}
			if (!defaultValuePtr)
			{
				throw std::runtime_error("the key doesn't exist and no default value was provided!");
			}
			obj = *defaultValuePtr;
		}
	}

	const nlohmann::json &_data;
	std::string           _name;
	bool                  _override_mode;
};
}        // namespace wvk::core