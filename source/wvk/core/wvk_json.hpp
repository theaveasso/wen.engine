#pragma once

#include <nlohmann/json.hpp>

#include "wvk/common/wvk_pch.hpp"
#include "wvk/common/wvk_defines.hpp"

#include "wvk_ldtk.hpp"

#ifndef WVK_JSON_ADAPTERS_HPP
#define WVK_JSON_ADAPTERS_HPP
namespace nlohmann
{
template <>
struct adl_serializer<std::filesystem::path>
{
	static void to_json(nlohmann::json &j, const std::filesystem::path &filepath)
	{
		j = filepath.string();
	}

	static void from_json(const nlohmann::json &j, std::filesystem::path &filepath)
	{
		filepath = std::filesystem::path(j.get<std::string>());
	}
};

template <typename T>
struct adl_serializer<std::shared_ptr<T>>
{
	static void to_json(json &j, const std::shared_ptr<T> &opt)
	{
		if (!opt)
			j = nullptr;
		else
			j = *opt;
	}

	static std::shared_ptr<T> from_json(const json &j)
	{
		if (j.is_null())
			return std::make_shared<T>();
		else
			return std::make_shared<T>(j.get<T>());
	}
};

template <typename T>
struct adl_serializer<std::optional<T>>
{
	static void to_json(json &j, const std::optional<T> &opt)
	{
		if (!opt)
			j = nullptr;
		else
			j = *opt;
	}

	static std::optional<T> from_json(const json &j)
	{
		if (j.is_null())
			return std::optional<T>();
		else
			return std::optional<T>(j.get<T>());
	}
};
}
#endif

namespace wvk::core
{
#ifndef WVK_UNTYPED_JSON_HELPER
#define WVK_UNTYPED_JSON_HELPER
inline json get_untyped(const json &j, const char *property)
{
	if (j.find(property) != j.end())
	{
		return j.at(property).get<json>();
	}
	return json();
}

inline json get_untyped(const json &j, std::string property)
{
	return get_untyped(j, property.data());
}
#endif

#ifndef WVK_OPTIONAL_JSON_HELPER
#define WVK_OPTIONAL_JSON_HELPER
template <typename T>
inline std::shared_ptr<T> get_heap_optional(const json &j, const char *property)
{
	auto it = j.find(property);
	if (it != j.end() && !it->is_null())
	{
		return j.at(property).get<std::shared_ptr<T>>();
	}
	return std::shared_ptr<T>();
}

template <typename T>
inline std::shared_ptr<T> get_heap_optional(const json &j, std::string property)
{
	return get_heap_optional<T>(j, property.data());
}
template <typename T>
inline std::optional<T> get_stack_optional(const json &j, const char *property)
{
	auto it = j.find(property);
	if (it != j.end() && !it->is_null())
	{
		return j.at(property).get<std::optional<T>>();
	}
	return std::optional<T>();
}

template <typename T>
inline std::optional<T> get_stack_optional(const json &j, std::string property)
{
	return get_stack_optional<T>(j, property.data());
}
#endif

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

void from_json(const json &j, LdtkCustomCommand &x);
void to_json(json &j, const LdtkCustomCommand &x);

void from_json(const json &j, FieldDefinition &x);
void to_json(json &j, const FieldDefinition &x);

void from_json(const json &j, TilesetRectangle &x);
void to_json(json &j, const TilesetRectangle &x);

void from_json(const json &j, EntityDefinition &x);
void to_json(json &j, const EntityDefinition &x);

void from_json(const json &j, EnumValueDefinition &x);
void to_json(json &j, const EnumValueDefinition &x);

void from_json(const json &j, EnumDefinition &x);
void to_json(json &j, const EnumDefinition &x);

void from_json(const json &j, AutoLayerRuleDefinition &x);
void to_json(json &j, const AutoLayerRuleDefinition &x);

void from_json(const json &j, AutoLayerRuleGroup &x);
void to_json(json &j, const AutoLayerRuleGroup &x);

void from_json(const json &j, IntGridValueDefinition &x);
void to_json(json &j, const IntGridValueDefinition &x);

void from_json(const json &j, IntGridValueGroupDefinition &x);
void to_json(json &j, const IntGridValueGroupDefinition &x);

void from_json(const json &j, LayerDefinition &x);
void to_json(json &j, const LayerDefinition &x);

void from_json(const json &j, TileCustomMetadata &x);
void to_json(json &j, const TileCustomMetadata &x);

void from_json(const json &j, EnumTagValue &x);
void to_json(json &j, const EnumTagValue &x);

void from_json(const json &j, TilesetDefinition &x);
void to_json(json &j, const TilesetDefinition &x);

void from_json(const json &j, LdtkDefs &x);
void to_json(json &j, const LdtkDefs &x);

void from_json(const json &j, FieldInstance &x);
void to_json(json &j, const FieldInstance &x);

void from_json(const json &j, EntityInstance &x);
void to_json(json &j, const EntityInstance &x);

void from_json(const json &j, ReferenceToAnEntityInstance &x);
void to_json(json &j, const ReferenceToAnEntityInstance &x);

void from_json(const json &j, GridPoint &x);
void to_json(json &j, const GridPoint &x);

void from_json(const json &j, IntGridValueInstance &x);
void to_json(json &j, const IntGridValueInstance &x);

void from_json(const json &j, TileInstance &x);
void to_json(json &j, const TileInstance &x);

void from_json(const json &j, LdtkLayer &x);
void to_json(json &j, const LdtkLayer &x);

void from_json(const json &j, LevelBackgroundPosition &x);
void to_json(json &j, const LevelBackgroundPosition &x);

void from_json(const json &j, NeighbourLevel &x);
void to_json(json &j, const NeighbourLevel &x);

void from_json(const json &j, LdtkLevel &x);
void to_json(json &j, const LdtkLevel &x);

void from_json(const json &j, LdtkTocInstanceData &x);
void to_json(json &j, const LdtkTocInstanceData &x);

void from_json(const json &j, LdtkTableOfContentEntry &x);
void to_json(json &j, const LdtkTableOfContentEntry &x);

void from_json(const json &j, LdtkWorld &x);
void to_json(json &j, const LdtkWorld &x);

void from_json(const json &j, ForcedRefs &x);
void to_json(json &j, const ForcedRefs &x);

void from_json(const json &j, LdtkJson &x);
void to_json(json &j, const LdtkJson &x);

void from_json(const json &j, When &x);
void to_json(json &j, const When &x);

void from_json(const json &j, AllowedRefs &x);
void to_json(json &j, const AllowedRefs &x);

void from_json(const json &j, EditorDisplayMode &x);
void to_json(json &j, const EditorDisplayMode &x);

void from_json(const json &j, EditorDisplayPos &x);
void to_json(json &j, const EditorDisplayPos &x);

void from_json(const json &j, EditorLinkStyle &x);
void to_json(json &j, const EditorLinkStyle &x);

void from_json(const json &j, TextLanguageMode &x);
void to_json(json &j, const TextLanguageMode &x);

void from_json(const json &j, LimitBehavior &x);
void to_json(json &j, const LimitBehavior &x);

void from_json(const json &j, LimitScope &x);
void to_json(json &j, const LimitScope &x);

void from_json(const json &j, RenderMode &x);
void to_json(json &j, const RenderMode &x);

void from_json(const json &j, TileRenderMode &x);
void to_json(json &j, const TileRenderMode &x);

void from_json(const json &j, Checker &x);
void to_json(json &j, const Checker &x);

void from_json(const json &j, TileMode &x);
void to_json(json &j, const TileMode &x);

void from_json(const json &j, Type &x);
void to_json(json &j, const Type &x);

void from_json(const json &j, EmbedAtlas &x);
void to_json(json &j, const EmbedAtlas &x);

void from_json(const json &j, Flag &x);
void to_json(json &j, const Flag &x);

void from_json(const json &j, BgPos &x);
void to_json(json &j, const BgPos &x);

void from_json(const json &j, WorldLayout &x);
void to_json(json &j, const WorldLayout &x);

void from_json(const json &j, IdentifierStyle &x);
void to_json(json &j, const IdentifierStyle &x);

void from_json(const json &j, ImageExportMode &x);
void to_json(json &j, const ImageExportMode &x);

inline void from_json(const json &j, LdtkCustomCommand &x)
{
	x.set_command(j.at("command").get<std::string>());
	x.set_when(j.at("when").get<When>());
}

inline void to_json(json &j, const LdtkCustomCommand &x)
{
	j            = json::object();
	j["command"] = x.get_command();
	j["when"]    = x.get_when();
}

inline void from_json(const json &j, FieldDefinition &x)
{
	x.set_type(j.at("__type").get<std::string>());
	x.set_accept_file_types(get_stack_optional<std::vector<std::string>>(j, "acceptFileTypes"));
	x.set_allowed_refs(j.at("allowedRefs").get<AllowedRefs>());
	x.set_allowed_refs_entity_uid(get_stack_optional<int64_t>(j, "allowedRefsEntityUid"));
	x.set_allowed_ref_tags(j.at("allowedRefTags").get<std::vector<std::string>>());
	x.set_allow_out_of_level_ref(j.at("allowOutOfLevelRef").get<bool>());
	x.set_array_max_length(get_stack_optional<int64_t>(j, "arrayMaxLength"));
	x.set_array_min_length(get_stack_optional<int64_t>(j, "arrayMinLength"));
	x.set_auto_chain_ref(j.at("autoChainRef").get<bool>());
	x.set_can_be_null(j.at("canBeNull").get<bool>());
	x.set_default_override(get_untyped(j, "defaultOverride"));
	x.set_doc(get_stack_optional<std::string>(j, "doc"));
	x.set_editor_always_show(j.at("editorAlwaysShow").get<bool>());
	x.set_editor_cut_long_values(j.at("editorCutLongValues").get<bool>());
	x.set_editor_display_color(get_stack_optional<std::string>(j, "editorDisplayColor"));
	x.set_editor_display_mode(j.at("editorDisplayMode").get<EditorDisplayMode>());
	x.set_editor_display_pos(j.at("editorDisplayPos").get<EditorDisplayPos>());
	x.set_editor_display_scale(j.at("editorDisplayScale").get<double>());
	x.set_editor_link_style(j.at("editorLinkStyle").get<EditorLinkStyle>());
	x.set_editor_show_in_world(j.at("editorShowInWorld").get<bool>());
	x.set_editor_text_prefix(get_stack_optional<std::string>(j, "editorTextPrefix"));
	x.set_editor_text_suffix(get_stack_optional<std::string>(j, "editorTextSuffix"));
	x.set_export_to_toc(j.at("exportToToc").get<bool>());
	x.set_identifier(j.at("identifier").get<std::string>());
	x.set_is_array(j.at("isArray").get<bool>());
	x.set_max(get_stack_optional<double>(j, "max"));
	x.set_min(get_stack_optional<double>(j, "min"));
	x.set_regex(get_stack_optional<std::string>(j, "regex"));
	x.set_searchable(j.at("searchable").get<bool>());
	x.set_symmetrical_ref(j.at("symmetricalRef").get<bool>());
	x.set_text_language_mode(get_stack_optional<TextLanguageMode>(j, "textLanguageMode"));
	x.set_tileset_uid(get_stack_optional<int64_t>(j, "tilesetUid"));
	x.set_field_definition_type(j.at("type").get<std::string>());
	x.set_uid(j.at("uid").get<int64_t>());
	x.set_use_for_smart_color(j.at("useForSmartColor").get<bool>());
}

inline void to_json(json &j, const FieldDefinition &x)
{
	j                         = json::object();
	j["__type"]               = x.get_type();
	j["acceptFileTypes"]      = x.get_accept_file_types();
	j["allowedRefs"]          = x.get_allowed_refs();
	j["allowedRefsEntityUid"] = x.get_allowed_refs_entity_uid();
	j["allowedRefTags"]       = x.get_allowed_ref_tags();
	j["allowOutOfLevelRef"]   = x.get_allow_out_of_level_ref();
	j["arrayMaxLength"]       = x.get_array_max_length();
	j["arrayMinLength"]       = x.get_array_min_length();
	j["autoChainRef"]         = x.get_auto_chain_ref();
	j["canBeNull"]            = x.get_can_be_null();
	j["defaultOverride"]      = x.get_default_override();
	j["doc"]                  = x.get_doc();
	j["editorAlwaysShow"]     = x.get_editor_always_show();
	j["editorCutLongValues"]  = x.get_editor_cut_long_values();
	j["editorDisplayColor"]   = x.get_editor_display_color();
	j["editorDisplayMode"]    = x.get_editor_display_mode();
	j["editorDisplayPos"]     = x.get_editor_display_pos();
	j["editorDisplayScale"]   = x.get_editor_display_scale();
	j["editorLinkStyle"]      = x.get_editor_link_style();
	j["editorShowInWorld"]    = x.get_editor_show_in_world();
	j["editorTextPrefix"]     = x.get_editor_text_prefix();
	j["editorTextSuffix"]     = x.get_editor_text_suffix();
	j["exportToToc"]          = x.get_export_to_toc();
	j["identifier"]           = x.get_identifier();
	j["isArray"]              = x.get_is_array();
	j["max"]                  = x.get_max();
	j["min"]                  = x.get_min();
	j["regex"]                = x.get_regex();
	j["searchable"]           = x.get_searchable();
	j["symmetricalRef"]       = x.get_symmetrical_ref();
	j["textLanguageMode"]     = x.get_text_language_mode();
	j["tilesetUid"]           = x.get_tileset_uid();
	j["type"]                 = x.get_field_definition_type();
	j["uid"]                  = x.get_uid();
	j["useForSmartColor"]     = x.get_use_for_smart_color();
}

inline void from_json(const json &j, TilesetRectangle &x)
{
	x.set_h(j.at("h").get<int64_t>());
	x.set_tileset_uid(j.at("tilesetUid").get<int64_t>());
	x.set_w(j.at("w").get<int64_t>());
	x.set_x(j.at("x").get<int64_t>());
	x.set_y(j.at("y").get<int64_t>());
}

inline void to_json(json &j, const TilesetRectangle &x)
{
	j               = json::object();
	j["h"]          = x.get_h();
	j["tilesetUid"] = x.get_tileset_uid();
	j["w"]          = x.get_w();
	j["x"]          = x.get_x();
	j["y"]          = x.get_y();
}

inline void from_json(const json &j, EntityDefinition &x)
{
	x.set_allow_out_of_bounds(j.at("allowOutOfBounds").get<bool>());
	x.set_color(j.at("color").get<std::string>());
	x.set_doc(get_stack_optional<std::string>(j, "doc"));
	x.set_export_to_toc(j.at("exportToToc").get<bool>());
	x.set_field_defs(j.at("fieldDefs").get<std::vector<FieldDefinition>>());
	x.set_fill_opacity(j.at("fillOpacity").get<double>());
	x.set_height(j.at("height").get<int64_t>());
	x.set_hollow(j.at("hollow").get<bool>());
	x.set_identifier(j.at("identifier").get<std::string>());
	x.set_keep_aspect_ratio(j.at("keepAspectRatio").get<bool>());
	x.set_limit_behavior(j.at("limitBehavior").get<LimitBehavior>());
	x.set_limit_scope(j.at("limitScope").get<LimitScope>());
	x.set_line_opacity(j.at("lineOpacity").get<double>());
	x.set_max_count(j.at("maxCount").get<int64_t>());
	x.set_max_height(get_stack_optional<int64_t>(j, "maxHeight"));
	x.set_max_width(get_stack_optional<int64_t>(j, "maxWidth"));
	x.set_min_height(get_stack_optional<int64_t>(j, "minHeight"));
	x.set_min_width(get_stack_optional<int64_t>(j, "minWidth"));
	x.set_nine_slice_borders(j.at("nineSliceBorders").get<std::vector<int64_t>>());
	x.set_pivot_x(j.at("pivotX").get<double>());
	x.set_pivot_y(j.at("pivotY").get<double>());
	x.set_render_mode(j.at("renderMode").get<RenderMode>());
	x.set_resizable_x(j.at("resizableX").get<bool>());
	x.set_resizable_y(j.at("resizableY").get<bool>());
	x.set_show_name(j.at("showName").get<bool>());
	x.set_tags(j.at("tags").get<std::vector<std::string>>());
	x.set_tile_id(get_stack_optional<int64_t>(j, "tileId"));
	x.set_tile_opacity(j.at("tileOpacity").get<double>());
	x.set_tile_rect(get_stack_optional<TilesetRectangle>(j, "tileRect"));
	x.set_tile_render_mode(j.at("tileRenderMode").get<TileRenderMode>());
	x.set_tileset_id(get_stack_optional<int64_t>(j, "tilesetId"));
	x.set_uid(j.at("uid").get<int64_t>());
	x.set_ui_tile_rect(get_stack_optional<TilesetRectangle>(j, "uiTileRect"));
	x.set_width(j.at("width").get<int64_t>());
}

inline void to_json(json &j, const EntityDefinition &x)
{
	j                     = json::object();
	j["allowOutOfBounds"] = x.get_allow_out_of_bounds();
	j["color"]            = x.get_color();
	j["doc"]              = x.get_doc();
	j["exportToToc"]      = x.get_export_to_toc();
	j["fieldDefs"]        = x.get_field_defs();
	j["fillOpacity"]      = x.get_fill_opacity();
	j["height"]           = x.get_height();
	j["hollow"]           = x.get_hollow();
	j["identifier"]       = x.get_identifier();
	j["keepAspectRatio"]  = x.get_keep_aspect_ratio();
	j["limitBehavior"]    = x.get_limit_behavior();
	j["limitScope"]       = x.get_limit_scope();
	j["lineOpacity"]      = x.get_line_opacity();
	j["maxCount"]         = x.get_max_count();
	j["maxHeight"]        = x.get_max_height();
	j["maxWidth"]         = x.get_max_width();
	j["minHeight"]        = x.get_min_height();
	j["minWidth"]         = x.get_min_width();
	j["nineSliceBorders"] = x.get_nine_slice_borders();
	j["pivotX"]           = x.get_pivot_x();
	j["pivotY"]           = x.get_pivot_y();
	j["renderMode"]       = x.get_render_mode();
	j["resizableX"]       = x.get_resizable_x();
	j["resizableY"]       = x.get_resizable_y();
	j["showName"]         = x.get_show_name();
	j["tags"]             = x.get_tags();
	j["tileId"]           = x.get_tile_id();
	j["tileOpacity"]      = x.get_tile_opacity();
	j["tileRect"]         = x.get_tile_rect();
	j["tileRenderMode"]   = x.get_tile_render_mode();
	j["tilesetId"]        = x.get_tileset_id();
	j["uid"]              = x.get_uid();
	j["uiTileRect"]       = x.get_ui_tile_rect();
	j["width"]            = x.get_width();
}

inline void from_json(const json &j, EnumValueDefinition &x)
{
	x.set_color(j.at("color").get<int64_t>());
	x.set_id(j.at("id").get<std::string>());
	x.set_tile_rect(get_stack_optional<TilesetRectangle>(j, "tileRect"));
}

inline void to_json(json &j, const EnumValueDefinition &x)
{
	j                  = json::object();
	j["color"]         = x.get_color();
	j["id"]            = x.get_id();
	j["tileRect"]      = x.get_tile_rect();
}

inline void from_json(const json &j, EnumDefinition &x)
{
	x.set_external_file_checksum(get_stack_optional<std::string>(j, "externalFileChecksum"));
	x.set_external_rel_path(get_stack_optional<std::string>(j, "externalRelPath"));
	x.set_icon_tileset_uid(get_stack_optional<int64_t>(j, "iconTilesetUid"));
	x.set_identifier(j.at("identifier").get<std::string>());
	x.set_tags(j.at("tags").get<std::vector<std::string>>());
	x.set_uid(j.at("uid").get<int64_t>());
	x.set_values(j.at("values").get<std::vector<EnumValueDefinition>>());
}

inline void to_json(json &j, const EnumDefinition &x)
{
	j                         = json::object();
	j["externalFileChecksum"] = x.get_external_file_checksum();
	j["externalRelPath"]      = x.get_external_rel_path();
	j["iconTilesetUid"]       = x.get_icon_tileset_uid();
	j["identifier"]           = x.get_identifier();
	j["tags"]                 = x.get_tags();
	j["uid"]                  = x.get_uid();
	j["values"]               = x.get_values();
}

inline void from_json(const json &j, AutoLayerRuleDefinition &x)
{
	x.set_active(j.at("active").get<bool>());
	x.set_alpha(j.at("alpha").get<double>());
	x.set_break_on_match(j.at("breakOnMatch").get<bool>());
	x.set_chance(j.at("chance").get<double>());
	x.set_checker(j.at("checker").get<Checker>());
	x.set_flip_x(j.at("flipX").get<bool>());
	x.set_flip_y(j.at("flipY").get<bool>());
	x.set_invalidated(j.at("invalidated").get<bool>());
	x.set_out_of_bounds_value(get_stack_optional<int64_t>(j, "outOfBoundsValue"));
	x.set_pattern(j.at("pattern").get<std::vector<int64_t>>());
	x.set_perlin_active(j.at("perlinActive").get<bool>());
	x.set_perlin_octaves(j.at("perlinOctaves").get<double>());
	x.set_perlin_scale(j.at("perlinScale").get<double>());
	x.set_perlin_seed(j.at("perlinSeed").get<double>());
	x.set_pivot_x(j.at("pivotX").get<double>());
	x.set_pivot_y(j.at("pivotY").get<double>());
	x.set_size(j.at("size").get<int64_t>());
	x.set_tile_mode(j.at("tileMode").get<TileMode>());
	x.set_tile_random_x_max(j.at("tileRandomXMax").get<int64_t>());
	x.set_tile_random_x_min(j.at("tileRandomXMin").get<int64_t>());
	x.set_tile_random_y_max(j.at("tileRandomYMax").get<int64_t>());
	x.set_tile_random_y_min(j.at("tileRandomYMin").get<int64_t>());
	x.set_tile_rects_ids(j.at("tileRectsIds").get<std::vector<std::vector<int64_t>>>());
	x.set_tile_x_offset(j.at("tileXOffset").get<int64_t>());
	x.set_tile_y_offset(j.at("tileYOffset").get<int64_t>());
	x.set_uid(j.at("uid").get<int64_t>());
	x.set_x_modulo(j.at("xModulo").get<int64_t>());
	x.set_x_offset(j.at("xOffset").get<int64_t>());
	x.set_y_modulo(j.at("yModulo").get<int64_t>());
	x.set_y_offset(j.at("yOffset").get<int64_t>());
}

inline void to_json(json &j, const AutoLayerRuleDefinition &x)
{
	j                     = json::object();
	j["active"]           = x.get_active();
	j["alpha"]            = x.get_alpha();
	j["breakOnMatch"]     = x.get_break_on_match();
	j["chance"]           = x.get_chance();
	j["checker"]          = x.get_checker();
	j["flipX"]            = x.get_flip_x();
	j["flipY"]            = x.get_flip_y();
	j["invalidated"]      = x.get_invalidated();
	j["outOfBoundsValue"] = x.get_out_of_bounds_value();
	j["pattern"]          = x.get_pattern();
	j["perlinActive"]     = x.get_perlin_active();
	j["perlinOctaves"]    = x.get_perlin_octaves();
	j["perlinScale"]      = x.get_perlin_scale();
	j["perlinSeed"]       = x.get_perlin_seed();
	j["pivotX"]           = x.get_pivot_x();
	j["pivotY"]           = x.get_pivot_y();
	j["size"]             = x.get_size();
	j["tileMode"]         = x.get_tile_mode();
	j["tileRandomXMax"]   = x.get_tile_random_x_max();
	j["tileRandomXMin"]   = x.get_tile_random_x_min();
	j["tileRandomYMax"]   = x.get_tile_random_y_max();
	j["tileRandomYMin"]   = x.get_tile_random_y_min();
	j["tileRectsIds"]     = x.get_tile_rects_ids();
	j["tileXOffset"]      = x.get_tile_x_offset();
	j["tileYOffset"]      = x.get_tile_y_offset();
	j["uid"]              = x.get_uid();
	j["xModulo"]          = x.get_x_modulo();
	j["xOffset"]          = x.get_x_offset();
	j["yModulo"]          = x.get_y_modulo();
	j["yOffset"]          = x.get_y_offset();
}

inline void from_json(const json &j, AutoLayerRuleGroup &x)
{
	x.set_active(j.at("active").get<bool>());
	x.set_biome_requirement_mode(j.at("biomeRequirementMode").get<int64_t>());
	x.set_collapsed(get_stack_optional<bool>(j, "collapsed"));
	x.set_color(get_stack_optional<std::string>(j, "color"));
	x.set_icon(get_stack_optional<TilesetRectangle>(j, "icon"));
	x.set_is_optional(j.at("isOptional").get<bool>());
	x.set_name(j.at("name").get<std::string>());
	x.set_required_biome_values(j.at("requiredBiomeValues").get<std::vector<std::string>>());
	x.set_rules(j.at("rules").get<std::vector<AutoLayerRuleDefinition>>());
	x.set_uid(j.at("uid").get<int64_t>());
	x.set_uses_wizard(j.at("usesWizard").get<bool>());
}

inline void to_json(json &j, const AutoLayerRuleGroup &x)
{
	j                         = json::object();
	j["active"]               = x.get_active();
	j["biomeRequirementMode"] = x.get_biome_requirement_mode();
	j["collapsed"]            = x.get_collapsed();
	j["color"]                = x.get_color();
	j["icon"]                 = x.get_icon();
	j["isOptional"]           = x.get_is_optional();
	j["name"]                 = x.get_name();
	j["requiredBiomeValues"]  = x.get_required_biome_values();
	j["rules"]                = x.get_rules();
	j["uid"]                  = x.get_uid();
	j["usesWizard"]           = x.get_uses_wizard();
}

inline void from_json(const json &j, IntGridValueDefinition &x)
{
	x.set_color(j.at("color").get<std::string>());
	x.set_group_uid(j.at("groupUid").get<int64_t>());
	x.set_identifier(get_stack_optional<std::string>(j, "identifier"));
	x.set_tile(get_stack_optional<TilesetRectangle>(j, "tile"));
	x.set_value(j.at("value").get<int64_t>());
}

inline void to_json(json &j, const IntGridValueDefinition &x)
{
	j               = json::object();
	j["color"]      = x.get_color();
	j["groupUid"]   = x.get_group_uid();
	j["identifier"] = x.get_identifier();
	j["tile"]       = x.get_tile();
	j["value"]      = x.get_value();
}

inline void from_json(const json &j, IntGridValueGroupDefinition &x)
{
	x.set_color(get_stack_optional<std::string>(j, "color"));
	x.set_identifier(get_stack_optional<std::string>(j, "identifier"));
	x.set_uid(j.at("uid").get<int64_t>());
}

inline void to_json(json &j, const IntGridValueGroupDefinition &x)
{
	j               = json::object();
	j["color"]      = x.get_color();
	j["identifier"] = x.get_identifier();
	j["uid"]        = x.get_uid();
}

inline void from_json(const json &j, LayerDefinition &x)
{
	x.set_type(j.at("__type").get<std::string>());
	x.set_auto_rule_groups(j.at("autoRuleGroups").get<std::vector<AutoLayerRuleGroup>>());
	x.set_auto_source_layer_def_uid(get_stack_optional<int64_t>(j, "autoSourceLayerDefUid"));
	x.set_auto_tileset_def_uid(get_stack_optional<int64_t>(j, "autoTilesetDefUid"));
	x.set_auto_tiles_killed_by_other_layer_uid(get_stack_optional<int64_t>(j, "autoTilesKilledByOtherLayerUid"));
	x.set_biome_field_uid(get_stack_optional<int64_t>(j, "biomeFieldUid"));
	x.set_can_select_when_inactive(j.at("canSelectWhenInactive").get<bool>());
	x.set_display_opacity(j.at("displayOpacity").get<double>());
	x.set_doc(get_stack_optional<std::string>(j, "doc"));
	x.set_excluded_tags(j.at("excludedTags").get<std::vector<std::string>>());
	x.set_grid_size(j.at("gridSize").get<int64_t>());
	x.set_guide_grid_hei(j.at("guideGridHei").get<int64_t>());
	x.set_guide_grid_wid(j.at("guideGridWid").get<int64_t>());
	x.set_hide_fields_when_inactive(j.at("hideFieldsWhenInactive").get<bool>());
	x.set_hide_in_list(j.at("hideInList").get<bool>());
	x.set_identifier(j.at("identifier").get<std::string>());
	x.set_inactive_opacity(j.at("inactiveOpacity").get<double>());
	x.set_int_grid_values(j.at("intGridValues").get<std::vector<IntGridValueDefinition>>());
	x.set_int_grid_values_groups(j.at("intGridValuesGroups").get<std::vector<IntGridValueGroupDefinition>>());
	x.set_parallax_factor_x(j.at("parallaxFactorX").get<double>());
	x.set_parallax_factor_y(j.at("parallaxFactorY").get<double>());
	x.set_parallax_scaling(j.at("parallaxScaling").get<bool>());
	x.set_px_offset_x(j.at("pxOffsetX").get<int64_t>());
	x.set_px_offset_y(j.at("pxOffsetY").get<int64_t>());
	x.set_render_in_world_view(j.at("renderInWorldView").get<bool>());
	x.set_required_tags(j.at("requiredTags").get<std::vector<std::string>>());
	x.set_tile_pivot_x(j.at("tilePivotX").get<double>());
	x.set_tile_pivot_y(j.at("tilePivotY").get<double>());
	x.set_tileset_def_uid(get_stack_optional<int64_t>(j, "tilesetDefUid"));
	x.set_layer_definition_type(j.at("type").get<Type>());
	x.set_ui_color(get_stack_optional<std::string>(j, "uiColor"));
	x.set_uid(j.at("uid").get<int64_t>());
	x.set_ui_filter_tags(j.at("uiFilterTags").get<std::vector<std::string>>());
	x.set_use_async_render(j.at("useAsyncRender").get<bool>());
}

inline void to_json(json &j, const LayerDefinition &x)
{
	j                                   = json::object();
	j["__type"]                         = x.get_type();
	j["autoRuleGroups"]                 = x.get_auto_rule_groups();
	j["autoSourceLayerDefUid"]          = x.get_auto_source_layer_def_uid();
	j["autoTilesetDefUid"]              = x.get_auto_tileset_def_uid();
	j["autoTilesKilledByOtherLayerUid"] = x.get_auto_tiles_killed_by_other_layer_uid();
	j["biomeFieldUid"]                  = x.get_biome_field_uid();
	j["canSelectWhenInactive"]          = x.get_can_select_when_inactive();
	j["displayOpacity"]                 = x.get_display_opacity();
	j["doc"]                            = x.get_doc();
	j["excludedTags"]                   = x.get_excluded_tags();
	j["gridSize"]                       = x.get_grid_size();
	j["guideGridHei"]                   = x.get_guide_grid_hei();
	j["guideGridWid"]                   = x.get_guide_grid_wid();
	j["hideFieldsWhenInactive"]         = x.get_hide_fields_when_inactive();
	j["hideInList"]                     = x.get_hide_in_list();
	j["identifier"]                     = x.get_identifier();
	j["inactiveOpacity"]                = x.get_inactive_opacity();
	j["intGridValues"]                  = x.get_int_grid_values();
	j["intGridValuesGroups"]            = x.get_int_grid_values_groups();
	j["parallaxFactorX"]                = x.get_parallax_factor_x();
	j["parallaxFactorY"]                = x.get_parallax_factor_y();
	j["parallaxScaling"]                = x.get_parallax_scaling();
	j["pxOffsetX"]                      = x.get_px_offset_x();
	j["pxOffsetY"]                      = x.get_px_offset_y();
	j["renderInWorldView"]              = x.get_render_in_world_view();
	j["requiredTags"]                   = x.get_required_tags();
	j["tilePivotX"]                     = x.get_tile_pivot_x();
	j["tilePivotY"]                     = x.get_tile_pivot_y();
	j["tilesetDefUid"]                  = x.get_tileset_def_uid();
	j["type"]                           = x.get_layer_definition_type();
	j["uiColor"]                        = x.get_ui_color();
	j["uid"]                            = x.get_uid();
	j["uiFilterTags"]                   = x.get_ui_filter_tags();
	j["useAsyncRender"]                 = x.get_use_async_render();
}

inline void from_json(const json &j, TileCustomMetadata &x)
{
	x.set_data(j.at("data").get<std::string>());
	x.set_tile_id(j.at("tileId").get<int64_t>());
}

inline void to_json(json &j, const TileCustomMetadata &x)
{
	j           = json::object();
	j["data"]   = x.get_data();
	j["tileId"] = x.get_tile_id();
}

inline void from_json(const json &j, EnumTagValue &x)
{
	x.set_enum_value_id(j.at("enumValueId").get<std::string>());
	x.set_tile_ids(j.at("tileIds").get<std::vector<int64_t>>());
}

inline void to_json(json &j, const EnumTagValue &x)
{
	j                = json::object();
	j["enumValueId"] = x.get_enum_value_id();
	j["tileIds"]     = x.get_tile_ids();
}

inline void from_json(const json &j, TilesetDefinition &x)
{
	x.set_c_hei(j.at("__cHei").get<int64_t>());
	x.set_c_wid(j.at("__cWid").get<int64_t>());
	x.set_cached_pixel_data(get_stack_optional<std::map<std::string, nlohmann::json>>(j, "cachedPixelData"));
	x.set_custom_data(j.at("customData").get<std::vector<TileCustomMetadata>>());
	x.set_embed_atlas(get_stack_optional<EmbedAtlas>(j, "embedAtlas"));
	x.set_enum_tags(j.at("enumTags").get<std::vector<EnumTagValue>>());
	x.set_identifier(j.at("identifier").get<std::string>());
	x.set_padding(j.at("padding").get<int64_t>());
	x.set_px_hei(j.at("pxHei").get<int64_t>());
	x.set_px_wid(j.at("pxWid").get<int64_t>());
	x.set_rel_path(get_stack_optional<std::string>(j, "relPath"));
	x.set_saved_selections(j.at("savedSelections").get<std::vector<std::map<std::string, nlohmann::json>>>());
	x.set_spacing(j.at("spacing").get<int64_t>());
	x.set_tags(j.at("tags").get<std::vector<std::string>>());
	x.set_tags_source_enum_uid(get_stack_optional<int64_t>(j, "tagsSourceEnumUid"));
	x.set_tile_grid_size(j.at("tileGridSize").get<int64_t>());
	x.set_uid(j.at("uid").get<int64_t>());
}

inline void to_json(json &j, const TilesetDefinition &x)
{
	j                      = json::object();
	j["__cHei"]            = x.get_c_hei();
	j["__cWid"]            = x.get_c_wid();
	j["cachedPixelData"]   = x.get_cached_pixel_data();
	j["customData"]        = x.get_custom_data();
	j["embedAtlas"]        = x.get_embed_atlas();
	j["enumTags"]          = x.get_enum_tags();
	j["identifier"]        = x.get_identifier();
	j["padding"]           = x.get_padding();
	j["pxHei"]             = x.get_px_hei();
	j["pxWid"]             = x.get_px_wid();
	j["relPath"]           = x.get_rel_path();
	j["savedSelections"]   = x.get_saved_selections();
	j["spacing"]           = x.get_spacing();
	j["tags"]              = x.get_tags();
	j["tagsSourceEnumUid"] = x.get_tags_source_enum_uid();
	j["tileGridSize"]      = x.get_tile_grid_size();
	j["uid"]               = x.get_uid();
}

inline void from_json(const json &j, LdtkDefs &x)
{
	x.set_entities(j.at("entities").get<std::vector<EntityDefinition>>());
	x.set_enums(j.at("enums").get<std::vector<EnumDefinition>>());
	x.set_external_enums(j.at("externalEnums").get<std::vector<EnumDefinition>>());
	x.set_layers(j.at("layers").get<std::vector<LayerDefinition>>());
	x.set_level_fields(j.at("levelFields").get<std::vector<FieldDefinition>>());
	x.set_tilesets(j.at("tilesets").get<std::vector<TilesetDefinition>>());
}

inline void to_json(json &j, const LdtkDefs &x)
{
	j                  = json::object();
	j["entities"]      = x.get_entities();
	j["enums"]         = x.get_enums();
	j["externalEnums"] = x.get_external_enums();
	j["layers"]        = x.get_layers();
	j["levelFields"]   = x.get_level_fields();
	j["tilesets"]      = x.get_tilesets();
}

inline void from_json(const json &j, FieldInstance &x)
{
	x.set_identifier(j.at("__identifier").get<std::string>());
	x.set_tile(get_stack_optional<TilesetRectangle>(j, "__tile"));
	x.set_type(j.at("__type").get<std::string>());
	x.set_value(get_untyped(j, "__value"));
	x.set_def_uid(j.at("defUid").get<int64_t>());
	x.set_real_editor_values(j.at("realEditorValues").get<std::vector<nlohmann::json>>());
}

inline void to_json(json &j, const FieldInstance &x)
{
	j                     = json::object();
	j["__identifier"]     = x.get_identifier();
	j["__tile"]           = x.get_tile();
	j["__type"]           = x.get_type();
	j["__value"]          = x.get_value();
	j["defUid"]           = x.get_def_uid();
	j["realEditorValues"] = x.get_real_editor_values();
}

inline void from_json(const json &j, EntityInstance &x)
{
	x.set_grid(j.at("__grid").get<std::vector<int64_t>>());
	x.set_identifier(j.at("__identifier").get<std::string>());
	x.set_pivot(j.at("__pivot").get<std::vector<double>>());
	x.set_smart_color(j.at("__smartColor").get<std::string>());
	x.set_tags(j.at("__tags").get<std::vector<std::string>>());
	x.set_tile(get_stack_optional<TilesetRectangle>(j, "__tile"));
	x.set_world_x(get_stack_optional<int64_t>(j, "__worldX"));
	x.set_world_y(get_stack_optional<int64_t>(j, "__worldY"));
	x.set_def_uid(j.at("defUid").get<int64_t>());
	x.set_field_instances(j.at("fieldInstances").get<std::vector<FieldInstance>>());
	x.set_height(j.at("height").get<int64_t>());
	x.set_iid(j.at("iid").get<std::string>());
	x.set_px(j.at("px").get<std::vector<int64_t>>());
	x.set_width(j.at("width").get<int64_t>());
}

inline void to_json(json &j, const EntityInstance &x)
{
	j                   = json::object();
	j["__grid"]         = x.get_grid();
	j["__identifier"]   = x.get_identifier();
	j["__pivot"]        = x.get_pivot();
	j["__smartColor"]   = x.get_smart_color();
	j["__tags"]         = x.get_tags();
	j["__tile"]         = x.get_tile();
	j["__worldX"]       = x.get_world_x();
	j["__worldY"]       = x.get_world_y();
	j["defUid"]         = x.get_def_uid();
	j["fieldInstances"] = x.get_field_instances();
	j["height"]         = x.get_height();
	j["iid"]            = x.get_iid();
	j["px"]             = x.get_px();
	j["width"]          = x.get_width();
}

inline void from_json(const json &j, ReferenceToAnEntityInstance &x)
{
	x.set_entity_iid(j.at("entityIid").get<std::string>());
	x.set_layer_iid(j.at("layerIid").get<std::string>());
	x.set_level_iid(j.at("levelIid").get<std::string>());
	x.set_world_iid(j.at("worldIid").get<std::string>());
}

inline void to_json(json &j, const ReferenceToAnEntityInstance &x)
{
	j              = json::object();
	j["entityIid"] = x.get_entity_iid();
	j["layerIid"]  = x.get_layer_iid();
	j["levelIid"]  = x.get_level_iid();
	j["worldIid"]  = x.get_world_iid();
}

inline void from_json(const json &j, GridPoint &x)
{
	x.set_cx(j.at("cx").get<int64_t>());
	x.set_cy(j.at("cy").get<int64_t>());
}

inline void to_json(json &j, const GridPoint &x)
{
	j       = json::object();
	j["cx"] = x.get_cx();
	j["cy"] = x.get_cy();
}

inline void from_json(const json &j, IntGridValueInstance &x)
{
	x.set_coord_id(j.at("coordId").get<int64_t>());
	x.set_v(j.at("v").get<int64_t>());
}

inline void to_json(json &j, const IntGridValueInstance &x)
{
	j            = json::object();
	j["coordId"] = x.get_coord_id();
	j["v"]       = x.get_v();
}

inline void from_json(const json &j, TileInstance &x)
{
	x.set_a(j.at("a").get<double>());
	x.set_d(j.at("d").get<std::vector<int64_t>>());
	x.set_f(j.at("f").get<int64_t>());
	x.set_px(j.at("px").get<std::vector<int64_t>>());
	x.set_src(j.at("src").get<std::vector<int64_t>>());
	x.set_t(j.at("t").get<int64_t>());
}

inline void to_json(json &j, const TileInstance &x)
{
	j        = json::object();
	j["a"]   = x.get_a();
	j["d"]   = x.get_d();
	j["f"]   = x.get_f();
	j["px"]  = x.get_px();
	j["src"] = x.get_src();
	j["t"]   = x.get_t();
}

inline void from_json(const json &j, LdtkLayer &x)
{
	x.set_c_hei(j.at("__cHei").get<int64_t>());
	x.set_c_wid(j.at("__cWid").get<int64_t>());
	x.set_grid_size(j.at("__gridSize").get<int64_t>());
	x.set_identifier(j.at("__identifier").get<std::string>());
	x.set_opacity(j.at("__opacity").get<double>());
	x.set_px_total_offset_x(j.at("__pxTotalOffsetX").get<int64_t>());
	x.set_px_total_offset_y(j.at("__pxTotalOffsetY").get<int64_t>());
	x.set_tileset_def_uid(get_stack_optional<int64_t>(j, "__tilesetDefUid"));
	x.set_tileset_rel_path(get_stack_optional<std::string>(j, "__tilesetRelPath"));
	x.set_type(j.at("__type").get<std::string>());
	x.set_auto_layer_tiles(j.at("autoLayerTiles").get<std::vector<TileInstance>>());
	x.set_entity_instances(j.at("entityInstances").get<std::vector<EntityInstance>>());
	x.set_grid_tiles(j.at("gridTiles").get<std::vector<TileInstance>>());
	x.set_iid(j.at("iid").get<std::string>());
	x.set_int_grid(get_stack_optional<std::vector<IntGridValueInstance>>(j, "intGrid"));
	x.set_int_grid_csv(j.at("intGridCsv").get<std::vector<int64_t>>());
	x.set_layer_def_uid(j.at("layerDefUid").get<int64_t>());
	x.set_level_id(j.at("levelId").get<int64_t>());
	x.set_optional_rules(j.at("optionalRules").get<std::vector<int64_t>>());
	x.set_override_tileset_uid(get_stack_optional<int64_t>(j, "overrideTilesetUid"));
	x.set_px_offset_x(j.at("pxOffsetX").get<int64_t>());
	x.set_px_offset_y(j.at("pxOffsetY").get<int64_t>());
	x.set_seed(j.at("seed").get<int64_t>());
	x.set_visible(j.at("visible").get<bool>());
}

inline void to_json(json &j, const LdtkLayer &x)
{
	j                       = json::object();
	j["__cHei"]             = x.get_c_hei();
	j["__cWid"]             = x.get_c_wid();
	j["__gridSize"]         = x.get_grid_size();
	j["__identifier"]       = x.get_identifier();
	j["__opacity"]          = x.get_opacity();
	j["__pxTotalOffsetX"]   = x.get_px_total_offset_x();
	j["__pxTotalOffsetY"]   = x.get_px_total_offset_y();
	j["__tilesetDefUid"]    = x.get_tileset_def_uid();
	j["__tilesetRelPath"]   = x.get_tileset_rel_path();
	j["__type"]             = x.get_type();
	j["autoLayerTiles"]     = x.get_auto_layer_tiles();
	j["entityInstances"]    = x.get_entity_instances();
	j["gridTiles"]          = x.get_grid_tiles();
	j["iid"]                = x.get_iid();
	j["intGrid"]            = x.get_int_grid();
	j["intGridCsv"]         = x.get_int_grid_csv();
	j["layerDefUid"]        = x.get_layer_def_uid();
	j["levelId"]            = x.get_level_id();
	j["optionalRules"]      = x.get_optional_rules();
	j["overrideTilesetUid"] = x.get_override_tileset_uid();
	j["pxOffsetX"]          = x.get_px_offset_x();
	j["pxOffsetY"]          = x.get_px_offset_y();
	j["seed"]               = x.get_seed();
	j["visible"]            = x.get_visible();
}

inline void from_json(const json &j, LevelBackgroundPosition &x)
{
	x.set_crop_rect(j.at("cropRect").get<std::vector<double>>());
	x.set_scale(j.at("scale").get<std::vector<double>>());
	x.set_top_left_px(j.at("topLeftPx").get<std::vector<int64_t>>());
}

inline void to_json(json &j, const LevelBackgroundPosition &x)
{
	j              = json::object();
	j["cropRect"]  = x.get_crop_rect();
	j["scale"]     = x.get_scale();
	j["topLeftPx"] = x.get_top_left_px();
}

inline void from_json(const json &j, NeighbourLevel &x)
{
	x.set_dir(j.at("dir").get<std::string>());
	x.set_level_iid(j.at("levelIid").get<std::string>());
}

inline void to_json(json &j, const NeighbourLevel &x)
{
	j             = json::object();
	j["dir"]      = x.get_dir();
	j["levelIid"] = x.get_level_iid();
}

inline void from_json(const json &j, LdtkLevel &x)
{
	x.set_bg_color(j.at("__bgColor").get<std::string>());
	x.set_bg_pos(get_stack_optional<LevelBackgroundPosition>(j, "__bgPos"));
	x.set_neighbours(j.at("__neighbours").get<std::vector<NeighbourLevel>>());
	x.set_smart_color(j.at("__smartColor").get<std::string>());
	x.set_level_bg_color(get_stack_optional<std::string>(j, "bgColor"));
	x.set_bg_pivot_x(j.at("bgPivotX").get<double>());
	x.set_bg_pivot_y(j.at("bgPivotY").get<double>());
	x.set_level_bg_pos(get_stack_optional<BgPos>(j, "bgPos"));
	x.set_bg_rel_path(get_stack_optional<std::string>(j, "bgRelPath"));
	x.set_external_rel_path(get_stack_optional<std::string>(j, "externalRelPath"));
	x.set_field_instances(j.at("fieldInstances").get<std::vector<FieldInstance>>());
	x.set_identifier(j.at("identifier").get<std::string>());
	x.set_iid(j.at("iid").get<std::string>());
	x.set_layer_instances(get_stack_optional<std::vector<LdtkLayer>>(j, "layerInstances"));
	x.set_px_hei(j.at("pxHei").get<int64_t>());
	x.set_px_wid(j.at("pxWid").get<int64_t>());
	x.set_uid(j.at("uid").get<int64_t>());
	x.set_use_auto_identifier(j.at("useAutoIdentifier").get<bool>());
	x.set_world_depth(j.at("worldDepth").get<int64_t>());
	x.set_world_x(j.at("worldX").get<int64_t>());
	x.set_world_y(j.at("worldY").get<int64_t>());
}

inline void to_json(json &j, const LdtkLevel &x)
{
	j                      = json::object();
	j["__bgColor"]         = x.get_bg_color();
	j["__bgPos"]           = x.get_bg_pos();
	j["__neighbours"]      = x.get_neighbours();
	j["__smartColor"]      = x.get_smart_color();
	j["bgColor"]           = x.get_level_bg_color();
	j["bgPivotX"]          = x.get_bg_pivot_x();
	j["bgPivotY"]          = x.get_bg_pivot_y();
	j["bgPos"]             = x.get_level_bg_pos();
	j["bgRelPath"]         = x.get_bg_rel_path();
	j["externalRelPath"]   = x.get_external_rel_path();
	j["fieldInstances"]    = x.get_field_instances();
	j["identifier"]        = x.get_identifier();
	j["iid"]               = x.get_iid();
	j["layerInstances"]    = x.get_layer_instances();
	j["pxHei"]             = x.get_px_hei();
	j["pxWid"]             = x.get_px_wid();
	j["uid"]               = x.get_uid();
	j["useAutoIdentifier"] = x.get_use_auto_identifier();
	j["worldDepth"]        = x.get_world_depth();
	j["worldX"]            = x.get_world_x();
	j["worldY"]            = x.get_world_y();
}

inline void from_json(const json &j, LdtkTocInstanceData &x)
{
	x.set_fields(get_untyped(j, "fields"));
	x.set_hei_px(j.at("heiPx").get<int64_t>());
	x.set_iids(j.at("iids").get<ReferenceToAnEntityInstance>());
	x.set_wid_px(j.at("widPx").get<int64_t>());
	x.set_world_x(j.at("worldX").get<int64_t>());
	x.set_world_y(j.at("worldY").get<int64_t>());
}

inline void to_json(json &j, const LdtkTocInstanceData &x)
{
	j           = json::object();
	j["fields"] = x.get_fields();
	j["heiPx"]  = x.get_hei_px();
	j["iids"]   = x.get_iids();
	j["widPx"]  = x.get_wid_px();
	j["worldX"] = x.get_world_x();
	j["worldY"] = x.get_world_y();
}

inline void from_json(const json &j, LdtkTableOfContentEntry &x)
{
	x.set_identifier(j.at("identifier").get<std::string>());
	x.set_instances(get_stack_optional<std::vector<ReferenceToAnEntityInstance>>(j, "instances"));
	x.set_instances_data(j.at("instancesData").get<std::vector<LdtkTocInstanceData>>());
}

inline void to_json(json &j, const LdtkTableOfContentEntry &x)
{
	j                  = json::object();
	j["identifier"]    = x.get_identifier();
	j["instances"]     = x.get_instances();
	j["instancesData"] = x.get_instances_data();
}

inline void from_json(const json &j, LdtkWorld &x)
{
	x.set_default_level_height(j.at("defaultLevelHeight").get<int64_t>());
	x.set_default_level_width(j.at("defaultLevelWidth").get<int64_t>());
	x.set_identifier(j.at("identifier").get<std::string>());
	x.set_iid(j.at("iid").get<std::string>());
	x.set_levels(j.at("levels").get<std::vector<LdtkLevel>>());
	x.set_world_grid_height(j.at("worldGridHeight").get<int64_t>());
	x.set_world_grid_width(j.at("worldGridWidth").get<int64_t>());
	x.set_world_layout(get_stack_optional<WorldLayout>(j, "worldLayout"));
}

inline void to_json(json &j, const LdtkWorld &x)
{
	j                       = json::object();
	j["defaultLevelHeight"] = x.get_default_level_height();
	j["defaultLevelWidth"]  = x.get_default_level_width();
	j["identifier"]         = x.get_identifier();
	j["iid"]                = x.get_iid();
	j["levels"]             = x.get_levels();
	j["worldGridHeight"]    = x.get_world_grid_height();
	j["worldGridWidth"]     = x.get_world_grid_width();
	j["worldLayout"]        = x.get_world_layout();
}

inline void from_json(const json &j, ForcedRefs &x)
{
	x.set_auto_layer_rule_group(get_stack_optional<AutoLayerRuleGroup>(j, "AutoLayerRuleGroup"));
	x.set_auto_rule_def(get_stack_optional<AutoLayerRuleDefinition>(j, "AutoRuleDef"));
	x.set_custom_command(get_stack_optional<LdtkCustomCommand>(j, "CustomCommand"));
	x.set_definitions(get_stack_optional<LdtkDefs>(j, "Definitions"));
	x.set_entity_def(get_stack_optional<EntityDefinition>(j, "EntityDef"));
	x.set_entity_instance(get_stack_optional<EntityInstance>(j, "EntityInstance"));
	x.set_entity_reference_infos(get_stack_optional<ReferenceToAnEntityInstance>(j, "EntityReferenceInfos"));
	x.set_enum_def(get_stack_optional<EnumDefinition>(j, "EnumDef"));
	x.set_enum_def_values(get_stack_optional<EnumValueDefinition>(j, "EnumDefValues"));
	x.set_enum_tag_value(get_stack_optional<EnumTagValue>(j, "EnumTagValue"));
	x.set_field_def(get_stack_optional<FieldDefinition>(j, "FieldDef"));
	x.set_field_instance(get_stack_optional<FieldInstance>(j, "FieldInstance"));
	x.set_grid_point(get_stack_optional<GridPoint>(j, "GridPoint"));
	x.set_int_grid_value_def(get_stack_optional<IntGridValueDefinition>(j, "IntGridValueDef"));
	x.set_int_grid_value_group_def(get_stack_optional<IntGridValueGroupDefinition>(j, "IntGridValueGroupDef"));
	x.set_int_grid_value_instance(get_stack_optional<IntGridValueInstance>(j, "IntGridValueInstance"));
	x.set_layer_def(get_stack_optional<LayerDefinition>(j, "LayerDef"));
	x.set_layer_instance(get_stack_optional<LdtkLayer>(j, "LdtkLayer"));
	x.set_level(get_stack_optional<LdtkLevel>(j, "Level"));
	x.set_level_bg_pos_infos(get_stack_optional<LevelBackgroundPosition>(j, "LevelBgPosInfos"));
	x.set_neighbour_level(get_stack_optional<NeighbourLevel>(j, "NeighbourLevel"));
	x.set_table_of_content_entry(get_stack_optional<LdtkTableOfContentEntry>(j, "TableOfContentEntry"));
	x.set_tile(get_stack_optional<TileInstance>(j, "Tile"));
	x.set_tile_custom_metadata(get_stack_optional<TileCustomMetadata>(j, "TileCustomMetadata"));
	x.set_tileset_def(get_stack_optional<TilesetDefinition>(j, "TilesetDef"));
	x.set_tileset_rect(get_stack_optional<TilesetRectangle>(j, "TilesetRect"));
	x.set_toc_instance_data(get_stack_optional<LdtkTocInstanceData>(j, "TocInstanceData"));
	x.set_world(get_stack_optional<LdtkWorld>(j, "World"));
}

inline void to_json(json &j, const ForcedRefs &x)
{
	j                         = json::object();
	j["AutoLayerRuleGroup"]   = x.get_auto_layer_rule_group();
	j["AutoRuleDef"]          = x.get_auto_rule_def();
	j["CustomCommand"]        = x.get_custom_command();
	j["Definitions"]          = x.get_definitions();
	j["EntityDef"]            = x.get_entity_def();
	j["EntityInstance"]       = x.get_entity_instance();
	j["EntityReferenceInfos"] = x.get_entity_reference_infos();
	j["EnumDef"]              = x.get_enum_def();
	j["EnumDefValues"]        = x.get_enum_def_values();
	j["EnumTagValue"]         = x.get_enum_tag_value();
	j["FieldDef"]             = x.get_field_def();
	j["FieldInstance"]        = x.get_field_instance();
	j["GridPoint"]            = x.get_grid_point();
	j["IntGridValueDef"]      = x.get_int_grid_value_def();
	j["IntGridValueGroupDef"] = x.get_int_grid_value_group_def();
	j["IntGridValueInstance"] = x.get_int_grid_value_instance();
	j["LayerDef"]             = x.get_layer_def();
	j["LdtkLayer"]        = x.get_layer_instance();
	j["Level"]                = x.get_level();
	j["LevelBgPosInfos"]      = x.get_level_bg_pos_infos();
	j["NeighbourLevel"]       = x.get_neighbour_level();
	j["TableOfContentEntry"]  = x.get_table_of_content_entry();
	j["Tile"]                 = x.get_tile();
	j["TileCustomMetadata"]   = x.get_tile_custom_metadata();
	j["TilesetDef"]           = x.get_tileset_def();
	j["TilesetRect"]          = x.get_tileset_rect();
	j["TocInstanceData"]      = x.get_toc_instance_data();
	j["World"]                = x.get_world();
}

inline void from_json(const json &j, LdtkJson &x)
{
	x.set_forced_refs(get_stack_optional<ForcedRefs>(j, "__FORCED_REFS"));
	x.set_app_build_id(j.at("appBuildId").get<double>());
	x.set_backup_limit(j.at("backupLimit").get<int64_t>());
	x.set_backup_on_save(j.at("backupOnSave").get<bool>());
	x.set_backup_rel_path(get_stack_optional<std::string>(j, "backupRelPath"));
	x.set_bg_color(j.at("bgColor").get<std::string>());
	x.set_custom_commands(j.at("customCommands").get<std::vector<LdtkCustomCommand>>());
	x.set_default_entity_height(j.at("defaultEntityHeight").get<int64_t>());
	x.set_default_entity_width(j.at("defaultEntityWidth").get<int64_t>());
	x.set_default_grid_size(j.at("defaultGridSize").get<int64_t>());
	x.set_default_level_bg_color(j.at("defaultLevelBgColor").get<std::string>());
	x.set_default_level_height(get_stack_optional<int64_t>(j, "defaultLevelHeight"));
	x.set_default_level_width(get_stack_optional<int64_t>(j, "defaultLevelWidth"));
	x.set_default_pivot_x(j.at("defaultPivotX").get<double>());
	x.set_default_pivot_y(j.at("defaultPivotY").get<double>());
	x.set_defs(j.at("defs").get<LdtkDefs>());
	x.set_dummy_world_iid(j.at("dummyWorldIid").get<std::string>());
	x.set_export_level_bg(j.at("exportLevelBg").get<bool>());
	x.set_export_png(get_stack_optional<bool>(j, "exportPng"));
	x.set_export_tiled(j.at("exportTiled").get<bool>());
	x.set_external_levels(j.at("externalLevels").get<bool>());
	x.set_flags(j.at("flags").get<std::vector<Flag>>());
	x.set_identifier_style(j.at("identifierStyle").get<IdentifierStyle>());
	x.set_iid(j.at("iid").get<std::string>());
	x.set_image_export_mode(j.at("imageExportMode").get<ImageExportMode>());
	x.set_json_version(j.at("jsonVersion").get<std::string>());
	x.set_level_name_pattern(j.at("levelNamePattern").get<std::string>());
	x.set_levels(j.at("levels").get<std::vector<LdtkLevel>>());
	x.set_minify_json(j.at("minifyJson").get<bool>());
	x.set_next_uid(j.at("nextUid").get<int64_t>());
	x.set_png_file_pattern(get_stack_optional<std::string>(j, "pngFilePattern"));
	x.set_simplified_export(j.at("simplifiedExport").get<bool>());
	x.set_toc(j.at("toc").get<std::vector<LdtkTableOfContentEntry>>());
	x.set_tutorial_desc(get_stack_optional<std::string>(j, "tutorialDesc"));
	x.set_world_grid_height(get_stack_optional<int64_t>(j, "worldGridHeight"));
	x.set_world_grid_width(get_stack_optional<int64_t>(j, "worldGridWidth"));
	x.set_world_layout(get_stack_optional<WorldLayout>(j, "worldLayout"));
	x.set_worlds(j.at("worlds").get<std::vector<LdtkWorld>>());
}

inline void to_json(json &j, const LdtkJson &x)
{
	j                        = json::object();
	j["__FORCED_REFS"]       = x.get_forced_refs();
	j["appBuildId"]          = x.get_app_build_id();
	j["backupLimit"]         = x.get_backup_limit();
	j["backupOnSave"]        = x.get_backup_on_save();
	j["backupRelPath"]       = x.get_backup_rel_path();
	j["bgColor"]             = x.get_bg_color();
	j["customCommands"]      = x.get_custom_commands();
	j["defaultEntityHeight"] = x.get_default_entity_height();
	j["defaultEntityWidth"]  = x.get_default_entity_width();
	j["defaultGridSize"]     = x.get_default_grid_size();
	j["defaultLevelBgColor"] = x.get_default_level_bg_color();
	j["defaultLevelHeight"]  = x.get_default_level_height();
	j["defaultLevelWidth"]   = x.get_default_level_width();
	j["defaultPivotX"]       = x.get_default_pivot_x();
	j["defaultPivotY"]       = x.get_default_pivot_y();
	j["defs"]                = x.get_defs();
	j["dummyWorldIid"]       = x.get_dummy_world_iid();
	j["exportLevelBg"]       = x.get_export_level_bg();
	j["exportPng"]           = x.get_export_png();
	j["exportTiled"]         = x.get_export_tiled();
	j["externalLevels"]      = x.get_external_levels();
	j["flags"]               = x.get_flags();
	j["identifierStyle"]     = x.get_identifier_style();
	j["iid"]                 = x.get_iid();
	j["imageExportMode"]     = x.get_image_export_mode();
	j["jsonVersion"]         = x.get_json_version();
	j["levelNamePattern"]    = x.get_level_name_pattern();
	j["levels"]              = x.get_levels();
	j["minifyJson"]          = x.get_minify_json();
	j["nextUid"]             = x.get_next_uid();
	j["pngFilePattern"]      = x.get_png_file_pattern();
	j["simplifiedExport"]    = x.get_simplified_export();
	j["toc"]                 = x.get_toc();
	j["tutorialDesc"]        = x.get_tutorial_desc();
	j["worldGridHeight"]     = x.get_world_grid_height();
	j["worldGridWidth"]      = x.get_world_grid_width();
	j["worldLayout"]         = x.get_world_layout();
	j["worlds"]              = x.get_worlds();
}

inline void from_json(const json &j, When &x)
{
	if (j == "AfterLoad")
		x = When::AFTER_LOAD;
	else if (j == "AfterSave")
		x = When::AFTER_SAVE;
	else if (j == "BeforeSave")
		x = When::BEFORE_SAVE;
	else if (j == "Manual")
		x = When::MANUAL;
	else
	{
		throw std::runtime_error("Input JSON does not conform to schema!");
	}
}

inline void to_json(json &j, const When &x)
{
	switch (x)
	{
		case When::AFTER_LOAD:
			j = "AfterLoad";
			break;
		case When::AFTER_SAVE:
			j = "AfterSave";
			break;
		case When::BEFORE_SAVE:
			j = "BeforeSave";
			break;
		case When::MANUAL:
			j = "Manual";
			break;
		default:
			throw std::runtime_error("This should not happen");
	}
}

inline void from_json(const json &j, AllowedRefs &x)
{
	if (j == "Any")
		x = AllowedRefs::ANY;
	else if (j == "OnlySame")
		x = AllowedRefs::ONLY_SAME;
	else if (j == "OnlySpecificEntity")
		x = AllowedRefs::ONLY_SPECIFIC_ENTITY;
	else if (j == "OnlyTags")
		x = AllowedRefs::ONLY_TAGS;
	else
	{
		throw std::runtime_error("Input JSON does not conform to schema!");
	}
}

inline void to_json(json &j, const AllowedRefs &x)
{
	switch (x)
	{
		case AllowedRefs::ANY:
			j = "Any";
			break;
		case AllowedRefs::ONLY_SAME:
			j = "OnlySame";
			break;
		case AllowedRefs::ONLY_SPECIFIC_ENTITY:
			j = "OnlySpecificEntity";
			break;
		case AllowedRefs::ONLY_TAGS:
			j = "OnlyTags";
			break;
		default:
			throw std::runtime_error("This should not happen");
	}
}

inline void from_json(const json &j, EditorDisplayMode &x)
{
	if (j == "ArrayCountNoLabel")
		x = EditorDisplayMode::ARRAY_COUNT_NO_LABEL;
	else if (j == "ArrayCountWithLabel")
		x = EditorDisplayMode::ARRAY_COUNT_WITH_LABEL;
	else if (j == "EntityTile")
		x = EditorDisplayMode::ENTITY_TILE;
	else if (j == "Hidden")
		x = EditorDisplayMode::HIDDEN;
	else if (j == "LevelTile")
		x = EditorDisplayMode::LEVEL_TILE;
	else if (j == "NameAndValue")
		x = EditorDisplayMode::NAME_AND_VALUE;
	else if (j == "Points")
		x = EditorDisplayMode::POINTS;
	else if (j == "PointPath")
		x = EditorDisplayMode::POINT_PATH;
	else if (j == "PointPathLoop")
		x = EditorDisplayMode::POINT_PATH_LOOP;
	else if (j == "PointStar")
		x = EditorDisplayMode::POINT_STAR;
	else if (j == "RadiusGrid")
		x = EditorDisplayMode::RADIUS_GRID;
	else if (j == "RadiusPx")
		x = EditorDisplayMode::RADIUS_PX;
	else if (j == "RefLinkBetweenCenters")
		x = EditorDisplayMode::REF_LINK_BETWEEN_CENTERS;
	else if (j == "RefLinkBetweenPivots")
		x = EditorDisplayMode::REF_LINK_BETWEEN_PIVOTS;
	else if (j == "ValueOnly")
		x = EditorDisplayMode::VALUE_ONLY;
	else
	{
		throw std::runtime_error("Input JSON does not conform to schema!");
	}
}

inline void to_json(json &j, const EditorDisplayMode &x)
{
	switch (x)
	{
		case EditorDisplayMode::ARRAY_COUNT_NO_LABEL:
			j = "ArrayCountNoLabel";
			break;
		case EditorDisplayMode::ARRAY_COUNT_WITH_LABEL:
			j = "ArrayCountWithLabel";
			break;
		case EditorDisplayMode::ENTITY_TILE:
			j = "EntityTile";
			break;
		case EditorDisplayMode::HIDDEN:
			j = "Hidden";
			break;
		case EditorDisplayMode::LEVEL_TILE:
			j = "LevelTile";
			break;
		case EditorDisplayMode::NAME_AND_VALUE:
			j = "NameAndValue";
			break;
		case EditorDisplayMode::POINTS:
			j = "Points";
			break;
		case EditorDisplayMode::POINT_PATH:
			j = "PointPath";
			break;
		case EditorDisplayMode::POINT_PATH_LOOP:
			j = "PointPathLoop";
			break;
		case EditorDisplayMode::POINT_STAR:
			j = "PointStar";
			break;
		case EditorDisplayMode::RADIUS_GRID:
			j = "RadiusGrid";
			break;
		case EditorDisplayMode::RADIUS_PX:
			j = "RadiusPx";
			break;
		case EditorDisplayMode::REF_LINK_BETWEEN_CENTERS:
			j = "RefLinkBetweenCenters";
			break;
		case EditorDisplayMode::REF_LINK_BETWEEN_PIVOTS:
			j = "RefLinkBetweenPivots";
			break;
		case EditorDisplayMode::VALUE_ONLY:
			j = "ValueOnly";
			break;
		default:
			throw std::runtime_error("This should not happen");
	}
}

inline void from_json(const json &j, EditorDisplayPos &x)
{
	if (j == "Above")
		x = EditorDisplayPos::ABOVE;
	else if (j == "Beneath")
		x = EditorDisplayPos::BENEATH;
	else if (j == "Center")
		x = EditorDisplayPos::CENTER;
	else
	{
		throw std::runtime_error("Input JSON does not conform to schema!");
	}
}

inline void to_json(json &j, const EditorDisplayPos &x)
{
	switch (x)
	{
		case EditorDisplayPos::ABOVE:
			j = "Above";
			break;
		case EditorDisplayPos::BENEATH:
			j = "Beneath";
			break;
		case EditorDisplayPos::CENTER:
			j = "Center";
			break;
		default:
			throw std::runtime_error("This should not happen");
	}
}

inline void from_json(const json &j, EditorLinkStyle &x)
{
	if (j == "ArrowsLine")
		x = EditorLinkStyle::ARROWS_LINE;
	else if (j == "CurvedArrow")
		x = EditorLinkStyle::CURVED_ARROW;
	else if (j == "DashedLine")
		x = EditorLinkStyle::DASHED_LINE;
	else if (j == "StraightArrow")
		x = EditorLinkStyle::STRAIGHT_ARROW;
	else if (j == "ZigZag")
		x = EditorLinkStyle::ZIG_ZAG;
	else
	{
		throw std::runtime_error("Input JSON does not conform to schema!");
	}
}

inline void to_json(json &j, const EditorLinkStyle &x)
{
	switch (x)
	{
		case EditorLinkStyle::ARROWS_LINE:
			j = "ArrowsLine";
			break;
		case EditorLinkStyle::CURVED_ARROW:
			j = "CurvedArrow";
			break;
		case EditorLinkStyle::DASHED_LINE:
			j = "DashedLine";
			break;
		case EditorLinkStyle::STRAIGHT_ARROW:
			j = "StraightArrow";
			break;
		case EditorLinkStyle::ZIG_ZAG:
			j = "ZigZag";
			break;
		default:
			throw std::runtime_error("This should not happen");
	}
}

inline void from_json(const json &j, TextLanguageMode &x)
{
	if (j == "LangC")
		x = TextLanguageMode::LANG_C;
	else if (j == "LangHaxe")
		x = TextLanguageMode::LANG_HAXE;
	else if (j == "LangJS")
		x = TextLanguageMode::LANG_JS;
	else if (j == "LangJson")
		x = TextLanguageMode::LANG_JSON;
	else if (j == "LangLog")
		x = TextLanguageMode::LANG_LOG;
	else if (j == "LangLua")
		x = TextLanguageMode::LANG_LUA;
	else if (j == "LangMarkdown")
		x = TextLanguageMode::LANG_MARKDOWN;
	else if (j == "LangPython")
		x = TextLanguageMode::LANG_PYTHON;
	else if (j == "LangRuby")
		x = TextLanguageMode::LANG_RUBY;
	else if (j == "LangXml")
		x = TextLanguageMode::LANG_XML;
	else
	{
		throw std::runtime_error("Input JSON does not conform to schema!");
	}
}

inline void to_json(json &j, const TextLanguageMode &x)
{
	switch (x)
	{
		case TextLanguageMode::LANG_C:
			j = "LangC";
			break;
		case TextLanguageMode::LANG_HAXE:
			j = "LangHaxe";
			break;
		case TextLanguageMode::LANG_JS:
			j = "LangJS";
			break;
		case TextLanguageMode::LANG_JSON:
			j = "LangJson";
			break;
		case TextLanguageMode::LANG_LOG:
			j = "LangLog";
			break;
		case TextLanguageMode::LANG_LUA:
			j = "LangLua";
			break;
		case TextLanguageMode::LANG_MARKDOWN:
			j = "LangMarkdown";
			break;
		case TextLanguageMode::LANG_PYTHON:
			j = "LangPython";
			break;
		case TextLanguageMode::LANG_RUBY:
			j = "LangRuby";
			break;
		case TextLanguageMode::LANG_XML:
			j = "LangXml";
			break;
		default:
			throw std::runtime_error("This should not happen");
	}
}

inline void from_json(const json &j, LimitBehavior &x)
{
	if (j == "DiscardOldOnes")
		x = LimitBehavior::DISCARD_OLD_ONES;
	else if (j == "MoveLastOne")
		x = LimitBehavior::MOVE_LAST_ONE;
	else if (j == "PreventAdding")
		x = LimitBehavior::PREVENT_ADDING;
	else
	{
		throw std::runtime_error("Input JSON does not conform to schema!");
	}
}

inline void to_json(json &j, const LimitBehavior &x)
{
	switch (x)
	{
		case LimitBehavior::DISCARD_OLD_ONES:
			j = "DiscardOldOnes";
			break;
		case LimitBehavior::MOVE_LAST_ONE:
			j = "MoveLastOne";
			break;
		case LimitBehavior::PREVENT_ADDING:
			j = "PreventAdding";
			break;
		default:
			throw std::runtime_error("This should not happen");
	}
}

inline void from_json(const json &j, LimitScope &x)
{
	if (j == "PerLayer")
		x = LimitScope::PER_LAYER;
	else if (j == "PerLevel")
		x = LimitScope::PER_LEVEL;
	else if (j == "PerWorld")
		x = LimitScope::PER_WORLD;
	else
	{
		throw std::runtime_error("Input JSON does not conform to schema!");
	}
}

inline void to_json(json &j, const LimitScope &x)
{
	switch (x)
	{
		case LimitScope::PER_LAYER:
			j = "PerLayer";
			break;
		case LimitScope::PER_LEVEL:
			j = "PerLevel";
			break;
		case LimitScope::PER_WORLD:
			j = "PerWorld";
			break;
		default:
			throw std::runtime_error("This should not happen");
	}
}

inline void from_json(const json &j, RenderMode &x)
{
	if (j == "Cross")
		x = RenderMode::CROSS;
	else if (j == "Ellipse")
		x = RenderMode::ELLIPSE;
	else if (j == "Rectangle")
		x = RenderMode::RECTANGLE;
	else if (j == "Tile")
		x = RenderMode::TILE;
	else
	{
		throw std::runtime_error("Input JSON does not conform to schema!");
	}
}

inline void to_json(json &j, const RenderMode &x)
{
	switch (x)
	{
		case RenderMode::CROSS:
			j = "Cross";
			break;
		case RenderMode::ELLIPSE:
			j = "Ellipse";
			break;
		case RenderMode::RECTANGLE:
			j = "Rectangle";
			break;
		case RenderMode::TILE:
			j = "Tile";
			break;
		default:
			throw std::runtime_error("This should not happen");
	}
}

inline void from_json(const json &j, TileRenderMode &x)
{
	if (j == "Cover")
		x = TileRenderMode::COVER;
	else if (j == "FitInside")
		x = TileRenderMode::FIT_INSIDE;
	else if (j == "FullSizeCropped")
		x = TileRenderMode::FULL_SIZE_CROPPED;
	else if (j == "FullSizeUncropped")
		x = TileRenderMode::FULL_SIZE_UNCROPPED;
	else if (j == "NineSlice")
		x = TileRenderMode::NINE_SLICE;
	else if (j == "Repeat")
		x = TileRenderMode::REPEAT;
	else if (j == "Stretch")
		x = TileRenderMode::STRETCH;
	else
	{
		throw std::runtime_error("Input JSON does not conform to schema!");
	}
}

inline void to_json(json &j, const TileRenderMode &x)
{
	switch (x)
	{
		case TileRenderMode::COVER:
			j = "Cover";
			break;
		case TileRenderMode::FIT_INSIDE:
			j = "FitInside";
			break;
		case TileRenderMode::FULL_SIZE_CROPPED:
			j = "FullSizeCropped";
			break;
		case TileRenderMode::FULL_SIZE_UNCROPPED:
			j = "FullSizeUncropped";
			break;
		case TileRenderMode::NINE_SLICE:
			j = "NineSlice";
			break;
		case TileRenderMode::REPEAT:
			j = "Repeat";
			break;
		case TileRenderMode::STRETCH:
			j = "Stretch";
			break;
		default:
			throw std::runtime_error("This should not happen");
	}
}

inline void from_json(const json &j, Checker &x)
{
	if (j == "Horizontal")
		x = Checker::HORIZONTAL;
	else if (j == "None")
		x = Checker::NONE;
	else if (j == "Vertical")
		x = Checker::VERTICAL;
	else
	{
		throw std::runtime_error("Input JSON does not conform to schema!");
	}
}

inline void to_json(json &j, const Checker &x)
{
	switch (x)
	{
		case Checker::HORIZONTAL:
			j = "Horizontal";
			break;
		case Checker::NONE:
			j = "None";
			break;
		case Checker::VERTICAL:
			j = "Vertical";
			break;
		default:
			throw std::runtime_error("This should not happen");
	}
}

inline void from_json(const json &j, TileMode &x)
{
	if (j == "Single")
		x = TileMode::SINGLE;
	else if (j == "Stamp")
		x = TileMode::STAMP;
	else
	{
		throw std::runtime_error("Input JSON does not conform to schema!");
	}
}

inline void to_json(json &j, const TileMode &x)
{
	switch (x)
	{
		case TileMode::SINGLE:
			j = "Single";
			break;
		case TileMode::STAMP:
			j = "Stamp";
			break;
		default:
			throw std::runtime_error("This should not happen");
	}
}

inline void from_json(const json &j, Type &x)
{
	if (j == "AutoLayer")
		x = Type::AUTO_LAYER;
	else if (j == "Entities")
		x = Type::ENTITIES;
	else if (j == "IntGrid")
		x = Type::INT_GRID;
	else if (j == "Tiles")
		x = Type::TILES;
	else
	{
		throw std::runtime_error("Input JSON does not conform to schema!");
	}
}

inline void to_json(json &j, const Type &x)
{
	switch (x)
	{
		case Type::AUTO_LAYER:
			j = "AutoLayer";
			break;
		case Type::ENTITIES:
			j = "Entities";
			break;
		case Type::INT_GRID:
			j = "IntGrid";
			break;
		case Type::TILES:
			j = "Tiles";
			break;
		default:
			throw std::runtime_error("This should not happen");
	}
}

inline void from_json(const json &j, EmbedAtlas &x)
{
	if (j == "LdtkIcons")
		x = EmbedAtlas::LDTK_ICONS;
	else
	{
		throw std::runtime_error("Input JSON does not conform to schema!");
	}
}

inline void to_json(json &j, const EmbedAtlas &x)
{
	switch (x)
	{
		case EmbedAtlas::LDTK_ICONS:
			j = "LdtkIcons";
			break;
		default:
			throw std::runtime_error("This should not happen");
	}
}

inline void from_json(const json &j, Flag &x)
{
	if (j == "DiscardPreCsvIntGrid")
		x = Flag::DISCARD_PRE_CSV_INT_GRID;
	else if (j == "ExportOldTableOfContentData")
		x = Flag::EXPORT_OLD_TABLE_OF_CONTENT_DATA;
	else if (j == "ExportPreCsvIntGridFormat")
		x = Flag::EXPORT_PRE_CSV_INT_GRID_FORMAT;
	else if (j == "IgnoreBackupSuggest")
		x = Flag::IGNORE_BACKUP_SUGGEST;
	else if (j == "MultiWorlds")
		x = Flag::MULTI_WORLDS;
	else if (j == "PrependIndexToLevelFileNames")
		x = Flag::PREPEND_INDEX_TO_LEVEL_FILE_NAMES;
	else if (j == "UseMultilinesType")
		x = Flag::USE_MULTILINES_TYPE;
	else
	{
		throw std::runtime_error("Input JSON does not conform to schema!");
	}
}

inline void to_json(json &j, const Flag &x)
{
	switch (x)
	{
		case Flag::DISCARD_PRE_CSV_INT_GRID:
			j = "DiscardPreCsvIntGrid";
			break;
		case Flag::EXPORT_OLD_TABLE_OF_CONTENT_DATA:
			j = "ExportOldTableOfContentData";
			break;
		case Flag::EXPORT_PRE_CSV_INT_GRID_FORMAT:
			j = "ExportPreCsvIntGridFormat";
			break;
		case Flag::IGNORE_BACKUP_SUGGEST:
			j = "IgnoreBackupSuggest";
			break;
		case Flag::MULTI_WORLDS:
			j = "MultiWorlds";
			break;
		case Flag::PREPEND_INDEX_TO_LEVEL_FILE_NAMES:
			j = "PrependIndexToLevelFileNames";
			break;
		case Flag::USE_MULTILINES_TYPE:
			j = "UseMultilinesType";
			break;
		default:
			throw std::runtime_error("This should not happen");
	}
}

inline void from_json(const json &j, BgPos &x)
{
	if (j == "Contain")
		x = BgPos::CONTAIN;
	else if (j == "Cover")
		x = BgPos::COVER;
	else if (j == "CoverDirty")
		x = BgPos::COVER_DIRTY;
	else if (j == "Repeat")
		x = BgPos::REPEAT;
	else if (j == "Unscaled")
		x = BgPos::UNSCALED;
	else
	{
		throw std::runtime_error("Input JSON does not conform to schema!");
	}
}

inline void to_json(json &j, const BgPos &x)
{
	switch (x)
	{
		case BgPos::CONTAIN:
			j = "Contain";
			break;
		case BgPos::COVER:
			j = "Cover";
			break;
		case BgPos::COVER_DIRTY:
			j = "CoverDirty";
			break;
		case BgPos::REPEAT:
			j = "Repeat";
			break;
		case BgPos::UNSCALED:
			j = "Unscaled";
			break;
		default:
			throw std::runtime_error("This should not happen");
	}
}

inline void from_json(const json &j, WorldLayout &x)
{
	if (j == "Free")
		x = WorldLayout::FREE;
	else if (j == "GridVania")
		x = WorldLayout::GRID_VANIA;
	else if (j == "LinearHorizontal")
		x = WorldLayout::LINEAR_HORIZONTAL;
	else if (j == "LinearVertical")
		x = WorldLayout::LINEAR_VERTICAL;
	else
	{
		throw std::runtime_error("Input JSON does not conform to schema!");
	}
}

inline void to_json(json &j, const WorldLayout &x)
{
	switch (x)
	{
		case WorldLayout::FREE:
			j = "Free";
			break;
		case WorldLayout::GRID_VANIA:
			j = "GridVania";
			break;
		case WorldLayout::LINEAR_HORIZONTAL:
			j = "LinearHorizontal";
			break;
		case WorldLayout::LINEAR_VERTICAL:
			j = "LinearVertical";
			break;
		default:
			throw std::runtime_error("This should not happen");
	}
}

inline void from_json(const json &j, IdentifierStyle &x)
{
	if (j == "Capitalize")
		x = IdentifierStyle::CAPITALIZE;
	else if (j == "Free")
		x = IdentifierStyle::FREE;
	else if (j == "Lowercase")
		x = IdentifierStyle::LOWERCASE;
	else if (j == "Uppercase")
		x = IdentifierStyle::UPPERCASE;
	else
	{
		throw std::runtime_error("Input JSON does not conform to schema!");
	}
}

inline void to_json(json &j, const IdentifierStyle &x)
{
	switch (x)
	{
		case IdentifierStyle::CAPITALIZE:
			j = "Capitalize";
			break;
		case IdentifierStyle::FREE:
			j = "Free";
			break;
		case IdentifierStyle::LOWERCASE:
			j = "Lowercase";
			break;
		case IdentifierStyle::UPPERCASE:
			j = "Uppercase";
			break;
		default:
			throw std::runtime_error("This should not happen");
	}
}

inline void from_json(const json &j, ImageExportMode &x)
{
	if (j == "LayersAndLevels")
		x = ImageExportMode::LAYERS_AND_LEVELS;
	else if (j == "None")
		x = ImageExportMode::NONE;
	else if (j == "OneImagePerLayer")
		x = ImageExportMode::ONE_IMAGE_PER_LAYER;
	else if (j == "OneImagePerLevel")
		x = ImageExportMode::ONE_IMAGE_PER_LEVEL;
	else
	{
		throw std::runtime_error("Input JSON does not conform to schema!");
	}
}

inline void to_json(json &j, const ImageExportMode &x)
{
	switch (x)
	{
		case ImageExportMode::LAYERS_AND_LEVELS:
			j = "LayersAndLevels";
			break;
		case ImageExportMode::NONE:
			j = "None";
			break;
		case ImageExportMode::ONE_IMAGE_PER_LAYER:
			j = "OneImagePerLayer";
			break;
		case ImageExportMode::ONE_IMAGE_PER_LEVEL:
			j = "OneImagePerLevel";
			break;
		default:
			throw std::runtime_error("This should not happen");
	}
}

}        // namespace wvk::core
