#pragma once

#include <glm/vec2.hpp>
#include <json.hpp>

#include "wvk/common/wvk_defines.hpp"
#include "wvk/common/wvk_pch.hpp"

namespace wvk
{

using TileId                              = int;
using TilesetId                           = int;
using TileIndex                           = glm::ivec2;
static constexpr TileId    NUL_TILE_ID    = -1;
static constexpr TilesetId NUL_TILESET_ID = -1;

struct Tile
{
	TileId    NULL_TILE_ID;
	TilesetId NULL_TILESET_ID;
};

}        // namespace wvk

namespace wvk::core
{

using nlohmann::json;
using std::optional;
using std::string;

using StringVec = std::vector<string>;

enum class When : int
{
	AFTER_LOAD,
	AFTER_SAVE,
	BEFORE_SAVE,
	MANUAL
};

class WVK_API LdtkCustomCommand
{
  public:
	LdtkCustomCommand()          = default;
	virtual ~LdtkCustomCommand() = default;

  private:
	string command;
	When   when = When::MANUAL;

  public:
	[[nodiscard]] const string &get_command() const { return command; }

	string &get_mutable_command() { return command; }

	void set_command(const string &value) { this->command = value; }

	[[nodiscard]] const When &get_when() const { return when; }

	When &get_mutable_when() { return when; }

	void set_when(const When &value) { this->when = value; }
};

enum class AllowedRefs : int
{
	ANY,
	ONLY_SAME,
	ONLY_SPECIFIC_ENTITY,
	ONLY_TAGS
};

enum class EditorDisplayMode : int
{
	ARRAY_COUNT_NO_LABEL,
	ARRAY_COUNT_WITH_LABEL,
	ENTITY_TILE,
	HIDDEN,
	LEVEL_TILE,
	NAME_AND_VALUE,
	POINTS,
	POINT_PATH,
	POINT_PATH_LOOP,
	POINT_STAR,
	RADIUS_GRID,
	RADIUS_PX,
	REF_LINK_BETWEEN_CENTERS,
	REF_LINK_BETWEEN_PIVOTS,
	VALUE_ONLY
};

enum class EditorDisplayPos : int
{
	ABOVE,
	BENEATH,
	CENTER
};

enum class EditorLinkStyle : int
{
	ARROWS_LINE,
	CURVED_ARROW,
	DASHED_LINE,
	STRAIGHT_ARROW,
	ZIG_ZAG
};

enum class TextLanguageMode : int
{
	LANG_C,
	LANG_HAXE,
	LANG_JS,
	LANG_JSON,
	LANG_LOG,
	LANG_LUA,
	LANG_MARKDOWN,
	LANG_PYTHON,
	LANG_RUBY,
	LANG_XML
};

class WVK_API FieldDefinition
{
  public:
	FieldDefinition()          = default;
	virtual ~FieldDefinition() = default;

  private:
	string                     type;
	optional<StringVec>        accept_file_types;
	AllowedRefs                allowed_refs{AllowedRefs::ANY};
	optional<int64_t>          allowed_refs_entity_uid;
	StringVec                  allowed_ref_tags;
	bool                       allow_out_of_level_ref{};
	optional<int64_t>          array_max_length;
	optional<int64_t>          array_min_length;
	bool                       auto_chain_ref{};
	bool                       can_be_null{};
	json                       default_override;
	optional<string>           doc;
	bool                       editor_always_show{};
	bool                       editor_cut_long_values{};
	optional<string>           editor_display_color;
	EditorDisplayMode          editor_display_mode{EditorDisplayMode::VALUE_ONLY};
	EditorDisplayPos           editor_display_pos{EditorDisplayPos::CENTER};
	double                     editor_display_scale{};
	EditorLinkStyle            editor_link_style{EditorLinkStyle::ZIG_ZAG};
	bool                       editor_show_in_world{};
	optional<string>           editor_text_prefix;
	optional<string>           editor_text_suffix;
	bool                       export_to_toc{};
	string                     identifier;
	bool                       is_array{};
	optional<double>           max;
	optional<double>           min;
	optional<string>           regex;
	bool                       searchable{};
	bool                       symmetrical_ref{};
	optional<TextLanguageMode> text_language_mode;
	optional<int64_t>          tileset_uid;
	string                     field_definition_type;
	int64_t                    uid{};
	bool                       use_for_smart_color{};

  public:
	[[nodiscard]] const string &get_type() const { return type; }

	string &get_mutable_type() { return type; }

	void set_type(const string &value) { this->type = value; }

	[[nodiscard]] optional<StringVec> get_accept_file_types() const { return accept_file_types; }

	void set_accept_file_types(optional<StringVec> value) { this->accept_file_types = std::move(value); }

	[[nodiscard]] const AllowedRefs &get_allowed_refs() const { return allowed_refs; }

	AllowedRefs &get_mutable_allowed_refs() { return allowed_refs; }

	void set_allowed_refs(const AllowedRefs &value) { this->allowed_refs = value; }

	[[nodiscard]] optional<int64_t> get_allowed_refs_entity_uid() const { return allowed_refs_entity_uid; }

	void set_allowed_refs_entity_uid(optional<int64_t> value) { this->allowed_refs_entity_uid = value; }

	[[nodiscard]] const StringVec &get_allowed_ref_tags() const { return allowed_ref_tags; }

	StringVec &get_mutable_allowed_ref_tags() { return allowed_ref_tags; }

	void set_allowed_ref_tags(const StringVec &value) { this->allowed_ref_tags = value; }

	[[nodiscard]] const bool &get_allow_out_of_level_ref() const { return allow_out_of_level_ref; }

	bool &get_mutable_allow_out_of_level_ref() { return allow_out_of_level_ref; }

	void set_allow_out_of_level_ref(const bool &value) { this->allow_out_of_level_ref = value; }

	[[nodiscard]] optional<int64_t> get_array_max_length() const { return array_max_length; }

	void set_array_max_length(optional<int64_t> value) { this->array_max_length = value; }

	[[nodiscard]] optional<int64_t> get_array_min_length() const { return array_min_length; }

	void set_array_min_length(optional<int64_t> value) { this->array_min_length = value; }

	[[nodiscard]] const bool &get_auto_chain_ref() const { return auto_chain_ref; }

	bool &get_mutable_auto_chain_ref() { return auto_chain_ref; }

	void set_auto_chain_ref(const bool &value) { this->auto_chain_ref = value; }

	[[nodiscard]] const bool &get_can_be_null() const { return can_be_null; }

	bool &get_mutable_can_be_null() { return can_be_null; }

	void set_can_be_null(const bool &value) { this->can_be_null = value; }

	[[nodiscard]] const json &get_default_override() const { return default_override; }

	json &get_mutable_default_override() { return default_override; }

	void set_default_override(const json &value) { this->default_override = value; }

	[[nodiscard]] optional<string> get_doc() const { return doc; }

	void set_doc(optional<string> value) { this->doc = std::move(value); }

	[[nodiscard]] const bool &get_editor_always_show() const { return editor_always_show; }

	bool &get_mutable_editor_always_show() { return editor_always_show; }

	void set_editor_always_show(const bool &value) { this->editor_always_show = value; }

	[[nodiscard]] const bool &get_editor_cut_long_values() const { return editor_cut_long_values; }

	bool &get_mutable_editor_cut_long_values() { return editor_cut_long_values; }

	void set_editor_cut_long_values(const bool &value) { this->editor_cut_long_values = value; }

	[[nodiscard]] optional<string> get_editor_display_color() const { return editor_display_color; }

	void set_editor_display_color(optional<string> value) { this->editor_display_color = std::move(value); }

	[[nodiscard]] const EditorDisplayMode &get_editor_display_mode() const { return editor_display_mode; }

	EditorDisplayMode &get_mutable_editor_display_mode() { return editor_display_mode; }

	void set_editor_display_mode(const EditorDisplayMode &value) { this->editor_display_mode = value; }

	[[nodiscard]] const EditorDisplayPos &get_editor_display_pos() const { return editor_display_pos; }

	EditorDisplayPos &get_mutable_editor_display_pos() { return editor_display_pos; }

	void set_editor_display_pos(const EditorDisplayPos &value) { this->editor_display_pos = value; }

	[[nodiscard]] const double &get_editor_display_scale() const { return editor_display_scale; }

	double &get_mutable_editor_display_scale() { return editor_display_scale; }

	void set_editor_display_scale(const double &value) { this->editor_display_scale = value; }

	[[nodiscard]] const EditorLinkStyle &get_editor_link_style() const { return editor_link_style; }

	EditorLinkStyle &get_mutable_editor_link_style() { return editor_link_style; }

	void set_editor_link_style(const EditorLinkStyle &value) { this->editor_link_style = value; }

	[[nodiscard]] const bool &get_editor_show_in_world() const { return editor_show_in_world; }

	bool &get_mutable_editor_show_in_world() { return editor_show_in_world; }

	void set_editor_show_in_world(const bool &value) { this->editor_show_in_world = value; }

	[[nodiscard]] optional<string> get_editor_text_prefix() const { return editor_text_prefix; }

	void set_editor_text_prefix(optional<string> value) { this->editor_text_prefix = std::move(value); }

	[[nodiscard]] optional<string> get_editor_text_suffix() const { return editor_text_suffix; }

	void set_editor_text_suffix(optional<string> value) { this->editor_text_suffix = std::move(value); }

	[[nodiscard]] const bool &get_export_to_toc() const { return export_to_toc; }

	bool &get_mutable_export_to_toc() { return export_to_toc; }

	void set_export_to_toc(const bool &value) { this->export_to_toc = value; }

	[[nodiscard]] const string &get_identifier() const { return identifier; }

	string &get_mutable_identifier() { return identifier; }

	void set_identifier(const string &value) { this->identifier = value; }

	[[nodiscard]] const bool &get_is_array() const { return is_array; }

	bool &get_mutable_is_array() { return is_array; }

	void set_is_array(const bool &value) { this->is_array = value; }

	[[nodiscard]] optional<double> get_max() const { return max; }

	void set_max(optional<double> value) { this->max = value; }

	[[nodiscard]] optional<double> get_min() const { return min; }

	void set_min(optional<double> value) { this->min = value; }

	[[nodiscard]] optional<string> get_regex() const { return regex; }

	void set_regex(optional<string> value) { this->regex = std::move(value); }

	[[nodiscard]] const bool &get_searchable() const { return searchable; }

	bool &get_mutable_searchable() { return searchable; }

	void set_searchable(const bool &value) { this->searchable = value; }

	[[nodiscard]] const bool &get_symmetrical_ref() const { return symmetrical_ref; }

	bool &get_mutable_symmetrical_ref() { return symmetrical_ref; }

	void set_symmetrical_ref(const bool &value) { this->symmetrical_ref = value; }

	[[nodiscard]] optional<TextLanguageMode> get_text_language_mode() const { return text_language_mode; }

	void set_text_language_mode(optional<TextLanguageMode> value) { this->text_language_mode = value; }

	[[nodiscard]] optional<int64_t> get_tileset_uid() const { return tileset_uid; }

	void set_tileset_uid(optional<int64_t> value) { this->tileset_uid = value; }

	[[nodiscard]] const string &get_field_definition_type() const { return field_definition_type; }

	string &get_mutable_field_definition_type() { return field_definition_type; }

	void set_field_definition_type(const string &value) { this->field_definition_type = value; }

	[[nodiscard]] const int64_t &get_uid() const { return uid; }

	int64_t &get_mutable_uid() { return uid; }

	void set_uid(const int64_t &value) { this->uid = value; }

	[[nodiscard]] const bool &get_use_for_smart_color() const { return use_for_smart_color; }

	bool &get_mutable_use_for_smart_color() { return use_for_smart_color; }

	void set_use_for_smart_color(const bool &value) { this->use_for_smart_color = value; }
};

enum class LimitBehavior : int
{
	DISCARD_OLD_ONES,
	MOVE_LAST_ONE,
	PREVENT_ADDING
};

enum class LimitScope : int
{
	PER_LAYER,
	PER_LEVEL,
	PER_WORLD
};

enum class RenderMode : int
{
	CROSS,
	ELLIPSE,
	RECTANGLE,
	TILE
};

/**
 * @brief This object represents a custom sub rectangle in a Tileset image.
 */
class WVK_API TilesetRectangle
{
  public:
	TilesetRectangle()          = default;
	virtual ~TilesetRectangle() = default;

  private:
	int64_t h           = {};
	int64_t tileset_uid = {};
	int64_t w           = {};
	int64_t x           = {};
	int64_t y           = {};

  public:
	/**
	 * Height in pixels
	 */
	[[nodiscard]] const int64_t &get_h() const { return h; }

	int64_t &get_mutable_h() { return h; }

	void set_h(const int64_t &value) { this->h = value; }

	/**
	 * UID of the tileset
	 */
	[[nodiscard]] const int64_t &get_tileset_uid() const { return tileset_uid; }

	int64_t &get_mutable_tileset_uid() { return tileset_uid; }

	void set_tileset_uid(const int64_t &value) { this->tileset_uid = value; }

	/**
	 * Width in pixels
	 */
	[[nodiscard]] const int64_t &get_w() const { return w; }

	int64_t &get_mutable_w() { return w; }

	void set_w(const int64_t &value) { this->w = value; }

	/**
	 * X pixels coordinate of the top-left corner in the Tileset image
	 */
	[[nodiscard]] const int64_t &get_x() const { return x; }

	int64_t &get_mutable_x() { return x; }

	void set_x(const int64_t &value) { this->x = value; }

	/**
	 * Y pixels coordinate of the top-left corner in the Tileset image
	 */
	[[nodiscard]] const int64_t &get_y() const { return y; }

	int64_t &get_mutable_y() { return y; }

	void set_y(const int64_t &value) { this->y = value; }
};

/**
 * @brief An enum describing how the the Entity tile is rendered inside the Entity bounds.
 */
enum class TileRenderMode : int
{
	COVER,
	FIT_INSIDE,
	FULL_SIZE_CROPPED,
	FULL_SIZE_UNCROPPED,
	NINE_SLICE,
	REPEAT,
	STRETCH
};

class WVK_API EntityDefinition
{
  public:
	EntityDefinition()          = default;
	virtual ~EntityDefinition() = default;

  private:
	bool                         allow_out_of_bounds{};
	string                       color;
	optional<string>             doc;
	bool                         export_to_toc{};
	std::vector<FieldDefinition> field_defs;
	double                       fill_opacity{};
	int64_t                      height{};
	bool                         hollow{};
	string                       identifier;
	bool                         keep_aspect_ratio{};
	LimitBehavior                limit_behavior{};
	LimitScope                   limit_scope{};
	double                       line_opacity{};
	int64_t                      max_count{};
	optional<int64_t>            max_height;
	optional<int64_t>            max_width;
	optional<int64_t>            min_height;
	optional<int64_t>            min_width;
	std::vector<int64_t>         nine_slice_borders;
	double                       pivot_x{};
	double                       pivot_y{};
	RenderMode                   render_mode{};
	bool                         resizable_x{};
	bool                         resizable_y{};
	bool                         show_name{};
	StringVec                    tags;
	optional<int64_t>            tile_id;
	double                       tile_opacity{};
	optional<TilesetRectangle>   tile_rect;
	TileRenderMode               tile_render_mode{};
	optional<int64_t>            tileset_id;
	int64_t                      uid{};
	optional<TilesetRectangle>   ui_tile_rect;
	int64_t                      width{};

  public:
	/**
	 * If enabled, this entity is allowed to stay outside of the current level bounds
	 */
	[[nodiscard]] const bool &get_allow_out_of_bounds() const { return allow_out_of_bounds; }

	bool &get_mutable_allow_out_of_bounds() { return allow_out_of_bounds; }

	void set_allow_out_of_bounds(const bool &value) { this->allow_out_of_bounds = value; }

	/**
	 * Base entity color
	 */
	[[nodiscard]] const string &get_color() const { return color; }

	string &get_mutable_color() { return color; }

	void set_color(const string &value) { this->color = value; }

	/**
	 * User defined documentation for this element to provide help/tips to level designers.
	 */
	[[nodiscard]] optional<string> get_doc() const { return doc; }

	void set_doc(optional<string> value) { this->doc = std::move(value); }

	/**
	 * If enabled, all instances of this entity will be listed in the project "Table of content"
	 * object.
	 */
	[[nodiscard]] const bool &get_export_to_toc() const { return export_to_toc; }

	bool &get_mutable_export_to_toc() { return export_to_toc; }

	void set_export_to_toc(const bool &value) { this->export_to_toc = value; }

	/**
	 * Array of field definitions
	 */
	[[nodiscard]] const std::vector<FieldDefinition> &get_field_defs() const { return field_defs; }

	std::vector<FieldDefinition> &get_mutable_field_defs() { return field_defs; }

	void set_field_defs(const std::vector<FieldDefinition> &value) { this->field_defs = value; }

	[[nodiscard]] const double &get_fill_opacity() const { return fill_opacity; }

	double &get_mutable_fill_opacity() { return fill_opacity; }

	void set_fill_opacity(const double &value) { this->fill_opacity = value; }

	/**
	 * Pixel height
	 */
	[[nodiscard]] const int64_t &get_height() const { return height; }

	int64_t &get_mutable_height() { return height; }

	void set_height(const int64_t &value) { this->height = value; }

	[[nodiscard]] const bool &get_hollow() const { return hollow; }

	bool &get_mutable_hollow() { return hollow; }

	void set_hollow(const bool &value) { this->hollow = value; }

	/**
	 * User defined unique identifier
	 */
	[[nodiscard]] const string &get_identifier() const { return identifier; }

	string &get_mutable_identifier() { return identifier; }

	void set_identifier(const string &value) { this->identifier = value; }

	/**
	 * Only applies to entities resizable on both X/Y. If TRUE, the entity instance width/height
	 * will keep the same aspect ratio as the definition.
	 */
	[[nodiscard]] const bool &get_keep_aspect_ratio() const { return keep_aspect_ratio; }

	bool &get_mutable_keep_aspect_ratio() { return keep_aspect_ratio; }

	void set_keep_aspect_ratio(const bool &value) { this->keep_aspect_ratio = value; }

	/**
	 * Possible values: `DiscardOldOnes`, `PreventAdding`, `MoveLastOne`
	 */
	[[nodiscard]] const LimitBehavior &get_limit_behavior() const { return limit_behavior; }

	LimitBehavior &get_mutable_limit_behavior() { return limit_behavior; }

	void set_limit_behavior(const LimitBehavior &value) { this->limit_behavior = value; }

	/**
	 * If TRUE, the maxCount is a "per world" limit, if FALSE, it's a "per level". Possible
	 * values: `PerLayer`, `PerLevel`, `PerWorld`
	 */
	[[nodiscard]] const LimitScope &get_limit_scope() const { return limit_scope; }

	LimitScope &get_mutable_limit_scope() { return limit_scope; }

	void set_limit_scope(const LimitScope &value) { this->limit_scope = value; }

	[[nodiscard]] const double &get_line_opacity() const { return line_opacity; }

	double &get_mutable_line_opacity() { return line_opacity; }

	void set_line_opacity(const double &value) { this->line_opacity = value; }

	/**
	 * Max instances count
	 */
	[[nodiscard]] const int64_t &get_max_count() const { return max_count; }

	int64_t &get_mutable_max_count() { return max_count; }

	void set_max_count(const int64_t &value) { this->max_count = value; }

	/**
	 * Max pixel height (only applies if the entity is resizable on Y)
	 */
	[[nodiscard]] optional<int64_t> get_max_height() const { return max_height; }

	void set_max_height(optional<int64_t> value) { this->max_height = value; }

	/**
	 * Max pixel width (only applies if the entity is resizable on X)
	 */
	[[nodiscard]] optional<int64_t> get_max_width() const { return max_width; }

	void set_max_width(optional<int64_t> value) { this->max_width = value; }

	/**
	 * Min pixel height (only applies if the entity is resizable on Y)
	 */
	[[nodiscard]] optional<int64_t> get_min_height() const { return min_height; }

	void set_min_height(optional<int64_t> value) { this->min_height = value; }

	/**
	 * Min pixel width (only applies if the entity is resizable on X)
	 */
	[[nodiscard]] optional<int64_t> get_min_width() const { return min_width; }

	void set_min_width(optional<int64_t> value) { this->min_width = value; }

	/**
	 * An array of 4 dimensions for the up/right/down/left borders (in this order) when using
	 * 9-slice mode for `tileRenderMode`.<br/>  If the tileRenderMode is not NineSlice, then
	 * this array is empty.<br/>  See: https://en.wikipedia.org/wiki/9-slice_scaling
	 */
	[[nodiscard]] const std::vector<int64_t> &get_nine_slice_borders() const { return nine_slice_borders; }

	std::vector<int64_t> &get_mutable_nine_slice_borders() { return nine_slice_borders; }

	void set_nine_slice_borders(const std::vector<int64_t> &value) { this->nine_slice_borders = value; }

	/**
	 * Pivot X coordinate (from 0 to 1.0)
	 */
	[[nodiscard]] const double &get_pivot_x() const { return pivot_x; }

	double &get_mutable_pivot_x() { return pivot_x; }

	void set_pivot_x(const double &value) { this->pivot_x = value; }

	/**
	 * Pivot Y coordinate (from 0 to 1.0)
	 */
	[[nodiscard]] const double &get_pivot_y() const { return pivot_y; }

	double &get_mutable_pivot_y() { return pivot_y; }

	void set_pivot_y(const double &value) { this->pivot_y = value; }

	/**
	 * Possible values: `Rectangle`, `Ellipse`, `Tile`, `Cross`
	 */
	[[nodiscard]] const RenderMode &get_render_mode() const { return render_mode; }

	RenderMode &get_mutable_render_mode() { return render_mode; }

	void set_render_mode(const RenderMode &value) { this->render_mode = value; }

	/**
	 * If TRUE, the entity instances will be resizable horizontally
	 */
	[[nodiscard]] const bool &get_resizable_x() const { return resizable_x; }

	bool &get_mutable_resizable_x() { return resizable_x; }

	void set_resizable_x(const bool &value) { this->resizable_x = value; }

	/**
	 * If TRUE, the entity instances will be resizable vertically
	 */
	[[nodiscard]] const bool &get_resizable_y() const { return resizable_y; }

	bool &get_mutable_resizable_y() { return resizable_y; }

	void set_resizable_y(const bool &value) { this->resizable_y = value; }

	/**
	 * Display entity name in editor
	 */
	[[nodiscard]] const bool &get_show_name() const { return show_name; }

	bool &get_mutable_show_name() { return show_name; }

	void set_show_name(const bool &value) { this->show_name = value; }

	/**
	 * An array of strings that class WVK_API ifies this entity
	 */
	[[nodiscard]] const StringVec &get_tags() const { return tags; }

	StringVec &get_mutable_tags() { return tags; }

	void set_tags(const StringVec &value) { this->tags = value; }

	/**
	 * **WARNING**: this deprecated value is no longer exported since version 1.2.0  Replaced
	 * by: `tileRect`
	 */
	[[nodiscard]] optional<int64_t> get_tile_id() const { return tile_id; }

	void set_tile_id(optional<int64_t> value) { this->tile_id = value; }

	[[nodiscard]] const double &get_tile_opacity() const { return tile_opacity; }

	double &get_mutable_tile_opacity() { return tile_opacity; }

	void set_tile_opacity(const double &value) { this->tile_opacity = value; }

	/**
	 * An object representing a rectangle from an existing Tileset
	 */
	[[nodiscard]] optional<TilesetRectangle> get_tile_rect() const { return tile_rect; }

	void set_tile_rect(optional<TilesetRectangle> value) { this->tile_rect = std::move(value); }

	/**
	 * An enum describing how the the Entity tile is rendered inside the Entity bounds. Possible
	 * values: `Cover`, `FitInside`, `Repeat`, `Stretch`, `FullSizeCropped`,
	 * `FullSizeUncropped`, `NineSlice`
	 */
	[[nodiscard]] const TileRenderMode &get_tile_render_mode() const { return tile_render_mode; }

	TileRenderMode &get_mutable_tile_render_mode() { return tile_render_mode; }

	void set_tile_render_mode(const TileRenderMode &value) { this->tile_render_mode = value; }

	/**
	 * Tileset ID used for optional tile display
	 */
	[[nodiscard]] optional<int64_t> get_tileset_id() const { return tileset_id; }

	void set_tileset_id(optional<int64_t> value) { this->tileset_id = value; }

	/**
	 * Unique Int identifier
	 */
	[[nodiscard]] const int64_t &get_uid() const { return uid; }

	int64_t &get_mutable_uid() { return uid; }

	void set_uid(const int64_t &value) { this->uid = value; }

	/**
	 * This tile overrides the one defined in `tileRect` in the UI
	 */
	[[nodiscard]] optional<TilesetRectangle> get_ui_tile_rect() const { return ui_tile_rect; }

	void set_ui_tile_rect(optional<TilesetRectangle> value) { this->ui_tile_rect = std::move(value); }

	/**
	 * Pixel width
	 */
	[[nodiscard]] const int64_t &get_width() const { return width; }

	int64_t &get_mutable_width() { return width; }

	void set_width(const int64_t &value) { this->width = value; }
};

class WVK_API EnumValueDefinition
{
  public:
	EnumValueDefinition()          = default;
	virtual ~EnumValueDefinition() = default;

  private:
	optional<std::vector<int64_t>> tile_src_rect;
	int64_t                        color{};
	string                         id;
	optional<int64_t>              tile_id;
	optional<TilesetRectangle>     tile_rect;

  public:
	/**
	 * Optional color
	 */
	[[nodiscard]] const int64_t &get_color() const { return color; }

	int64_t &get_mutable_color() { return color; }

	void set_color(const int64_t &value) { this->color = value; }

	/**
	 * Enum value
	 */
	[[nodiscard]] const string &get_id() const { return id; }

	string &get_mutable_id() { return id; }

	void set_id(const string &value) { this->id = value; }

	/**
	 * Optional tileset rectangle to represents this value
	 */
	[[nodiscard]] optional<TilesetRectangle> get_tile_rect() const { return tile_rect; }

	void set_tile_rect(optional<TilesetRectangle> value) { this->tile_rect = std::move(value); }
};

class WVK_API EnumDefinition
{
  public:
	EnumDefinition()          = default;
	virtual ~EnumDefinition() = default;

  private:
	optional<string>                 external_file_checksum;
	optional<string>                 external_rel_path;
	optional<int64_t>                icon_tileset_uid;
	string                           identifier;
	StringVec                        tags;
	int64_t                          uid{};
	std::vector<EnumValueDefinition> values;

  public:
	[[nodiscard]] optional<string> get_external_file_checksum() const { return external_file_checksum; }

	void set_external_file_checksum(optional<string> value) { this->external_file_checksum = std::move(value); }

	/**
	 * Relative path to the external file providing this Enum
	 */
	[[nodiscard]] optional<string> get_external_rel_path() const { return external_rel_path; }

	void set_external_rel_path(optional<string> value) { this->external_rel_path = std::move(value); }

	/**
	 * Tileset UID if provided
	 */
	[[nodiscard]] optional<int64_t> get_icon_tileset_uid() const { return icon_tileset_uid; }

	void set_icon_tileset_uid(optional<int64_t> value) { this->icon_tileset_uid = value; }

	/**
	 * User defined unique identifier
	 */
	[[nodiscard]] const string &get_identifier() const { return identifier; }

	string &get_mutable_identifier() { return identifier; }

	void set_identifier(const string &value) { this->identifier = value; }

	/**
	 * An array of user-defined tags to organize the Enums
	 */
	[[nodiscard]] const StringVec &get_tags() const { return tags; }

	StringVec &get_mutable_tags() { return tags; }

	void set_tags(const StringVec &value) { this->tags = value; }

	/**
	 * Unique Int identifier
	 */
	[[nodiscard]] const int64_t &get_uid() const { return uid; }

	int64_t &get_mutable_uid() { return uid; }

	void set_uid(const int64_t &value) { this->uid = value; }

	/**
	 * All possible enum values, with their optional Tile infos.
	 */
	[[nodiscard]] const std::vector<EnumValueDefinition> &get_values() const { return values; }

	std::vector<EnumValueDefinition> &get_mutable_values() { return values; }

	void set_values(const std::vector<EnumValueDefinition> &value) { this->values = value; }
};

/**
 * Checker mode Possible values: `None`, `Horizontal`, `Vertical`
 */
enum class Checker : int
{
	HORIZONTAL,
	NONE,
	VERTICAL
};

/**
 * Defines how tileIds array is used Possible values: `Single`, `Stamp`
 */
enum class TileMode : int
{
	SINGLE,
	STAMP
};

/**
 * This complex section isn't meant to be used by game devs at all, as these rules are
 * completely resolved internally by the editor before any saving. You should just ignore
 * this part.
 */
class WVK_API AutoLayerRuleDefinition
{
  public:
	AutoLayerRuleDefinition()          = default;
	virtual ~AutoLayerRuleDefinition() = default;

  private:
	bool                              active{};
	double                            alpha{};
	bool                              break_on_match{};
	double                            chance{};
	Checker                           checker{};
	bool                              flip_x{};
	bool                              flip_y{};
	bool                              invalidated{};
	optional<int64_t>                 out_of_bounds_value;
	std::vector<int64_t>              pattern;
	bool                              perlin_active{};
	double                            perlin_octaves{};
	double                            perlin_scale{};
	double                            perlin_seed{};
	double                            pivot_x{};
	double                            pivot_y{};
	int64_t                           size{};
	optional<std::vector<int64_t>>    tile_ids;
	TileMode                          tile_mode{};
	int64_t                           tile_random_x_max{};
	int64_t                           tile_random_x_min{};
	int64_t                           tile_random_y_max{};
	int64_t                           tile_random_y_min{};
	std::vector<std::vector<int64_t>> tile_rects_ids;
	int64_t                           tile_x_offset{};
	int64_t                           tile_y_offset{};
	int64_t                           uid{};
	int64_t                           x_modulo{};
	int64_t                           x_offset{};
	int64_t                           y_modulo{};
	int64_t                           y_offset{};

  public:
	/**
	 * If FALSE, the rule effect isn't applied, and no tiles are generated.
	 */
	[[nodiscard]] const bool &get_active() const { return active; }

	bool &get_mutable_active() { return active; }

	void set_active(const bool &value) { this->active = value; }

	[[nodiscard]] const double &get_alpha() const { return alpha; }

	double &get_mutable_alpha() { return alpha; }

	void set_alpha(const double &value) { this->alpha = value; }

	/**
	 * When TRUE, the rule will prevent other rules to be applied in the same cell if it matches
	 * (TRUE by default).
	 */
	[[nodiscard]] const bool &get_break_on_match() const { return break_on_match; }

	bool &get_mutable_break_on_match() { return break_on_match; }

	void set_break_on_match(const bool &value) { this->break_on_match = value; }

	/**
	 * Chances for this rule to be applied (0 to 1)
	 */
	[[nodiscard]] const double &get_chance() const { return chance; }

	double &get_mutable_chance() { return chance; }

	void set_chance(const double &value) { this->chance = value; }

	/**
	 * Checker mode Possible values: `None`, `Horizontal`, `Vertical`
	 */
	[[nodiscard]] const Checker &get_checker() const { return checker; }

	Checker &get_mutable_checker() { return checker; }

	void set_checker(const Checker &value) { this->checker = value; }

	/**
	 * If TRUE, allow rule to be matched by flipping its pattern horizontally
	 */
	[[nodiscard]] const bool &get_flip_x() const { return flip_x; }

	bool &get_mutable_flip_x() { return flip_x; }

	void set_flip_x(const bool &value) { this->flip_x = value; }

	/**
	 * If TRUE, allow rule to be matched by flipping its pattern vertically
	 */
	[[nodiscard]] const bool &get_flip_y() const { return flip_y; }

	bool &get_mutable_flip_y() { return flip_y; }

	void set_flip_y(const bool &value) { this->flip_y = value; }

	/**
	 * If TRUE, then the rule should be re-evaluated by the editor at one point
	 */
	[[nodiscard]] const bool &get_invalidated() const { return invalidated; }

	bool &get_mutable_invalidated() { return invalidated; }

	void set_invalidated(const bool &value) { this->invalidated = value; }

	/**
	 * Default IntGrid value when checking cells outside of level bounds
	 */
	[[nodiscard]] optional<int64_t> get_out_of_bounds_value() const { return out_of_bounds_value; }

	void set_out_of_bounds_value(optional<int64_t> value) { this->out_of_bounds_value = value; }

	/**
	 * Rule pattern (size x size)
	 */
	[[nodiscard]] const std::vector<int64_t> &get_pattern() const { return pattern; }

	std::vector<int64_t> &get_mutable_pattern() { return pattern; }

	void set_pattern(const std::vector<int64_t> &value) { this->pattern = value; }

	/**
	 * If TRUE, enable Perlin filtering to only apply rule on specific random area
	 */
	[[nodiscard]] const bool &get_perlin_active() const { return perlin_active; }

	bool &get_mutable_perlin_active() { return perlin_active; }

	void set_perlin_active(const bool &value) { this->perlin_active = value; }

	[[nodiscard]] const double &get_perlin_octaves() const { return perlin_octaves; }

	double &get_mutable_perlin_octaves() { return perlin_octaves; }

	void set_perlin_octaves(const double &value) { this->perlin_octaves = value; }

	[[nodiscard]] const double &get_perlin_scale() const { return perlin_scale; }

	double &get_mutable_perlin_scale() { return perlin_scale; }

	void set_perlin_scale(const double &value) { this->perlin_scale = value; }

	[[nodiscard]] const double &get_perlin_seed() const { return perlin_seed; }

	double &get_mutable_perlin_seed() { return perlin_seed; }

	void set_perlin_seed(const double &value) { this->perlin_seed = value; }

	/**
	 * X pivot of a tile stamp (0-1)
	 */
	[[nodiscard]] const double &get_pivot_x() const { return pivot_x; }

	double &get_mutable_pivot_x() { return pivot_x; }

	void set_pivot_x(const double &value) { this->pivot_x = value; }

	/**
	 * Y pivot of a tile stamp (0-1)
	 */
	[[nodiscard]] const double &get_pivot_y() const { return pivot_y; }

	double &get_mutable_pivot_y() { return pivot_y; }

	void set_pivot_y(const double &value) { this->pivot_y = value; }

	/**
	 * Pattern width & height. Should only be 1,3,5 or 7.
	 */
	[[nodiscard]] const int64_t &get_size() const { return size; }

	int64_t &get_mutable_size() { return size; }

	void set_size(const int64_t &value) { this->size = value; }

	/**
	 * Defines how tileIds array is used Possible values: `Single`, `Stamp`
	 */
	[[nodiscard]] const TileMode &get_tile_mode() const { return tile_mode; }

	TileMode &get_mutable_tile_mode() { return tile_mode; }

	void set_tile_mode(const TileMode &value) { this->tile_mode = value; }

	/**
	 * Max random offset for X tile pos
	 */
	[[nodiscard]] const int64_t &get_tile_random_x_max() const { return tile_random_x_max; }

	int64_t &get_mutable_tile_random_x_max() { return tile_random_x_max; }

	void set_tile_random_x_max(const int64_t &value) { this->tile_random_x_max = value; }

	/**
	 * Min random offset for X tile pos
	 */
	[[nodiscard]] const int64_t &get_tile_random_x_min() const { return tile_random_x_min; }

	int64_t &get_mutable_tile_random_x_min() { return tile_random_x_min; }

	void set_tile_random_x_min(const int64_t &value) { this->tile_random_x_min = value; }

	/**
	 * Max random offset for Y tile pos
	 */
	[[nodiscard]] const int64_t &get_tile_random_y_max() const { return tile_random_y_max; }

	int64_t &get_mutable_tile_random_y_max() { return tile_random_y_max; }

	void set_tile_random_y_max(const int64_t &value) { this->tile_random_y_max = value; }

	/**
	 * Min random offset for Y tile pos
	 */
	[[nodiscard]] const int64_t &get_tile_random_y_min() const { return tile_random_y_min; }

	int64_t &get_mutable_tile_random_y_min() { return tile_random_y_min; }

	void set_tile_random_y_min(const int64_t &value) { this->tile_random_y_min = value; }

	/**
	 * Array containing all the possible tile IDs rectangles (picked randomly).
	 */
	[[nodiscard]] const std::vector<std::vector<int64_t>> &get_tile_rects_ids() const { return tile_rects_ids; }

	std::vector<std::vector<int64_t>> &get_mutable_tile_rects_ids() { return tile_rects_ids; }

	void set_tile_rects_ids(const std::vector<std::vector<int64_t>> &value) { this->tile_rects_ids = value; }

	/**
	 * Tile X offset
	 */
	[[nodiscard]] const int64_t &get_tile_x_offset() const { return tile_x_offset; }

	int64_t &get_mutable_tile_x_offset() { return tile_x_offset; }

	void set_tile_x_offset(const int64_t &value) { this->tile_x_offset = value; }

	/**
	 * Tile Y offset
	 */
	[[nodiscard]] const int64_t &get_tile_y_offset() const { return tile_y_offset; }

	int64_t &get_mutable_tile_y_offset() { return tile_y_offset; }

	void set_tile_y_offset(const int64_t &value) { this->tile_y_offset = value; }

	/**
	 * Unique Int identifier
	 */
	[[nodiscard]] const int64_t &get_uid() const { return uid; }

	int64_t &get_mutable_uid() { return uid; }

	void set_uid(const int64_t &value) { this->uid = value; }

	/**
	 * X cell coord modulo
	 */
	[[nodiscard]] const int64_t &get_x_modulo() const { return x_modulo; }

	int64_t &get_mutable_x_modulo() { return x_modulo; }

	void set_x_modulo(const int64_t &value) { this->x_modulo = value; }

	/**
	 * X cell start offset
	 */
	[[nodiscard]] const int64_t &get_x_offset() const { return x_offset; }

	int64_t &get_mutable_x_offset() { return x_offset; }

	void set_x_offset(const int64_t &value) { this->x_offset = value; }

	/**
	 * Y cell coord modulo
	 */
	[[nodiscard]] const int64_t &get_y_modulo() const { return y_modulo; }

	int64_t &get_mutable_y_modulo() { return y_modulo; }

	void set_y_modulo(const int64_t &value) { this->y_modulo = value; }

	/**
	 * Y cell start offset
	 */
	[[nodiscard]] const int64_t &get_y_offset() const { return y_offset; }

	int64_t &get_mutable_y_offset() { return y_offset; }

	void set_y_offset(const int64_t &value) { this->y_offset = value; }
};

class WVK_API AutoLayerRuleGroup
{
  public:
	AutoLayerRuleGroup()          = default;
	virtual ~AutoLayerRuleGroup() = default;

  private:
	bool                                 active{};
	int64_t                              biome_requirement_mode{};
	optional<bool>                       collapsed;
	optional<string>                     color;
	optional<TilesetRectangle>           icon;
	bool                                 is_optional{};
	string                               name;
	StringVec                            required_biome_values;
	std::vector<AutoLayerRuleDefinition> rules;
	int64_t                              uid{};
	bool                                 uses_wizard{};

  public:
	[[nodiscard]] const bool &get_active() const { return active; }

	bool &get_mutable_active() { return active; }

	void set_active(const bool &value) { this->active = value; }

	[[nodiscard]] const int64_t &get_biome_requirement_mode() const { return biome_requirement_mode; }

	int64_t &get_mutable_biome_requirement_mode() { return biome_requirement_mode; }

	void set_biome_requirement_mode(const int64_t &value) { this->biome_requirement_mode = value; }

	/**
	 * *This field was removed in 1.0.0 and should no longer be used.*
	 */
	[[nodiscard]] optional<bool> get_collapsed() const { return collapsed; }

	void set_collapsed(optional<bool> value) { this->collapsed = value; }

	[[nodiscard]] optional<string> get_color() const { return color; }

	void set_color(optional<string> value) { this->color = std::move(value); }

	[[nodiscard]] optional<TilesetRectangle> get_icon() const { return icon; }

	void set_icon(optional<TilesetRectangle> value) { this->icon = std::move(value); }

	[[nodiscard]] const bool &get_is_optional() const { return is_optional; }

	bool &get_mutable_is_optional() { return is_optional; }

	void set_is_optional(const bool &value) { this->is_optional = value; }

	[[nodiscard]] const string &get_name() const { return name; }

	string &get_mutable_name() { return name; }

	void set_name(const string &value) { this->name = value; }

	[[nodiscard]] const StringVec &get_required_biome_values() const { return required_biome_values; }

	StringVec &get_mutable_required_biome_values() { return required_biome_values; }

	void set_required_biome_values(const StringVec &value) { this->required_biome_values = value; }

	[[nodiscard]] const std::vector<AutoLayerRuleDefinition> &get_rules() const { return rules; }

	std::vector<AutoLayerRuleDefinition> &get_mutable_rules() { return rules; }

	void set_rules(const std::vector<AutoLayerRuleDefinition> &value) { this->rules = value; }

	[[nodiscard]] const int64_t &get_uid() const { return uid; }

	int64_t &get_mutable_uid() { return uid; }

	void set_uid(const int64_t &value) { this->uid = value; }

	[[nodiscard]] const bool &get_uses_wizard() const { return uses_wizard; }

	bool &get_mutable_uses_wizard() { return uses_wizard; }

	void set_uses_wizard(const bool &value) { this->uses_wizard = value; }
};

/**
 * IntGrid value definition
 */
class WVK_API IntGridValueDefinition
{
  public:
	IntGridValueDefinition()          = default;
	virtual ~IntGridValueDefinition() = default;

  private:
	string                     color;
	int64_t                    group_uid{};
	optional<string>           identifier;
	optional<TilesetRectangle> tile;
	int64_t                    value{};

  public:
	[[nodiscard]] const string &get_color() const { return color; }

	string &get_mutable_color() { return color; }

	void set_color(const string &v) { this->color = v; }

	/**
	 * Parent group identifier (0 if none)
	 */
	[[nodiscard]] const int64_t &get_group_uid() const { return group_uid; }

	int64_t &get_mutable_group_uid() { return group_uid; }

	void set_group_uid(const int64_t &v) { this->group_uid = v; }

	/**
	 * User defined unique identifier
	 */
	[[nodiscard]] optional<string> get_identifier() const { return identifier; }

	void set_identifier(optional<string> v) { this->identifier = std::move(v); }

	[[nodiscard]] optional<TilesetRectangle> get_tile() const { return tile; }

	void set_tile(optional<TilesetRectangle> v) { this->tile = std::move(v); }

	/**
	 * The IntGrid value itself
	 */
	[[nodiscard]] const int64_t &get_value() const { return value; }

	int64_t &get_mutable_value() { return value; }

	void set_value(const int64_t &v) { this->value = v; }
};

/**
 * IntGrid value group definition
 */
class WVK_API IntGridValueGroupDefinition
{
  public:
	IntGridValueGroupDefinition()          = default;
	virtual ~IntGridValueGroupDefinition() = default;

  private:
	optional<string> color;
	optional<string> identifier;
	int64_t          uid{};

  public:
	/**
	 * User defined color
	 */
	[[nodiscard]] optional<string> get_color() const { return color; }

	void set_color(optional<string> value) { this->color = std::move(value); }

	/**
	 * User defined string identifier
	 */
	[[nodiscard]] optional<string> get_identifier() const { return identifier; }

	void set_identifier(optional<string> value) { this->identifier = std::move(value); }

	/**
	 * Group unique ID
	 */
	[[nodiscard]] const int64_t &get_uid() const { return uid; }

	int64_t &get_mutable_uid() { return uid; }

	void set_uid(const int64_t &value) { this->uid = value; }
};

/**
 * Type of the layer as Haxe Enum Possible values: `IntGrid`, `Entities`, `Tiles`,
 * `AutoLayer`
 */
enum class Type : int
{
	AUTO_LAYER,
	ENTITIES,
	INT_GRID,
	TILES
};

class WVK_API LayerDefinition
{
  public:
	LayerDefinition()          = default;
	virtual ~LayerDefinition() = default;

  private:
	string                                   type;
	std::vector<AutoLayerRuleGroup>          auto_rule_groups;
	optional<int64_t>                        auto_source_layer_def_uid;
	optional<int64_t>                        auto_tileset_def_uid;
	optional<int64_t>                        auto_tiles_killed_by_other_layer_uid;
	optional<int64_t>                        biome_field_uid;
	bool                                     can_select_when_inactive{};
	double                                   display_opacity{};
	optional<string>                         doc;
	StringVec                                excluded_tags;
	int64_t                                  grid_size{};
	int64_t                                  guide_grid_hei{};
	int64_t                                  guide_grid_wid{};
	bool                                     hide_fields_when_inactive{};
	bool                                     hide_in_list{};
	string                                   identifier;
	double                                   inactive_opacity{};
	std::vector<IntGridValueDefinition>      int_grid_values;
	std::vector<IntGridValueGroupDefinition> int_grid_values_groups;
	double                                   parallax_factor_x{};
	double                                   parallax_factor_y{};
	bool                                     parallax_scaling{};
	int64_t                                  px_offset_x{};
	int64_t                                  px_offset_y{};
	bool                                     render_in_world_view{};
	StringVec                                required_tags;
	double                                   tile_pivot_x{};
	double                                   tile_pivot_y{};
	optional<int64_t>                        tileset_def_uid;
	Type                                     layer_definition_type{};
	optional<string>                         ui_color;
	int64_t                                  uid{};
	StringVec                                ui_filter_tags;
	bool                                     use_async_render{};

  public:
	/**
	 * Type of the layer (*IntGrid, Entities, Tiles or AutoLayer*)
	 */
	[[nodiscard]] const string &get_type() const { return type; }

	string &get_mutable_type() { return type; }

	void set_type(const string &value) { this->type = value; }

	/**
	 * Contains all the auto-layer rule definitions.
	 */
	[[nodiscard]] const std::vector<AutoLayerRuleGroup> &get_auto_rule_groups() const { return auto_rule_groups; }

	std::vector<AutoLayerRuleGroup> &get_mutable_auto_rule_groups() { return auto_rule_groups; }

	void set_auto_rule_groups(const std::vector<AutoLayerRuleGroup> &value) { this->auto_rule_groups = value; }

	[[nodiscard]] optional<int64_t> get_auto_source_layer_def_uid() const { return auto_source_layer_def_uid; }

	void set_auto_source_layer_def_uid(optional<int64_t> value) { this->auto_source_layer_def_uid = value; }

	/**
	 * **WARNING**: this deprecated value is no longer exported since version 1.2.0  Replaced
	 * by: `tilesetDefUid`
	 */
	[[nodiscard]] optional<int64_t> get_auto_tileset_def_uid() const { return auto_tileset_def_uid; }

	void set_auto_tileset_def_uid(optional<int64_t> value) { this->auto_tileset_def_uid = value; }

	[[nodiscard]] optional<int64_t> get_auto_tiles_killed_by_other_layer_uid() const { return auto_tiles_killed_by_other_layer_uid; }

	void set_auto_tiles_killed_by_other_layer_uid(optional<int64_t> value) { this->auto_tiles_killed_by_other_layer_uid = value; }

	[[nodiscard]] optional<int64_t> get_biome_field_uid() const { return biome_field_uid; }

	void set_biome_field_uid(optional<int64_t> value) { this->biome_field_uid = value; }

	/**
	 * Allow editor selections when the layer is not currently active.
	 */
	[[nodiscard]] const bool &get_can_select_when_inactive() const { return can_select_when_inactive; }

	bool &get_mutable_can_select_when_inactive() { return can_select_when_inactive; }

	void set_can_select_when_inactive(const bool &value) { this->can_select_when_inactive = value; }

	/**
	 * Opacity of the layer (0 to 1.0)
	 */
	[[nodiscard]] const double &get_display_opacity() const { return display_opacity; }

	double &get_mutable_display_opacity() { return display_opacity; }

	void set_display_opacity(const double &value) { this->display_opacity = value; }

	/**
	 * User defined documentation for this element to provide help/tips to level designers.
	 */
	[[nodiscard]] optional<string> get_doc() const { return doc; }

	void set_doc(optional<string> value) { this->doc = std::move(value); }

	/**
	 * An array of tags to forbid some Entities in this layer
	 */
	[[nodiscard]] const StringVec &get_excluded_tags() const { return excluded_tags; }

	StringVec &get_mutable_excluded_tags() { return excluded_tags; }

	void set_excluded_tags(const StringVec &value) { this->excluded_tags = value; }

	/**
	 * Width and height of the grid in pixels
	 */
	[[nodiscard]] const int64_t &get_grid_size() const { return grid_size; }

	int64_t &get_mutable_grid_size() { return grid_size; }

	void set_grid_size(const int64_t &value) { this->grid_size = value; }

	/**
	 * Height of the optional "guide" grid in pixels
	 */
	[[nodiscard]] const int64_t &get_guide_grid_hei() const { return guide_grid_hei; }

	int64_t &get_mutable_guide_grid_hei() { return guide_grid_hei; }

	void set_guide_grid_hei(const int64_t &value) { this->guide_grid_hei = value; }

	/**
	 * Width of the optional "guide" grid in pixels
	 */
	[[nodiscard]] const int64_t &get_guide_grid_wid() const { return guide_grid_wid; }

	int64_t &get_mutable_guide_grid_wid() { return guide_grid_wid; }

	void set_guide_grid_wid(const int64_t &value) { this->guide_grid_wid = value; }

	[[nodiscard]] const bool &get_hide_fields_when_inactive() const { return hide_fields_when_inactive; }

	bool &get_mutable_hide_fields_when_inactive() { return hide_fields_when_inactive; }

	void set_hide_fields_when_inactive(const bool &value) { this->hide_fields_when_inactive = value; }

	/**
	 * Hide the layer from the list on the side of the editor view.
	 */
	[[nodiscard]] const bool &get_hide_in_list() const { return hide_in_list; }

	bool &get_mutable_hide_in_list() { return hide_in_list; }

	void set_hide_in_list(const bool &value) { this->hide_in_list = value; }

	/**
	 * User defined unique identifier
	 */
	[[nodiscard]] const string &get_identifier() const { return identifier; }

	string &get_mutable_identifier() { return identifier; }

	void set_identifier(const string &value) { this->identifier = value; }

	/**
	 * Alpha of this layer when it is not the active one.
	 */
	[[nodiscard]] const double &get_inactive_opacity() const { return inactive_opacity; }

	double &get_mutable_inactive_opacity() { return inactive_opacity; }

	void set_inactive_opacity(const double &value) { this->inactive_opacity = value; }

	/**
	 * An array that defines extra optional info for each IntGrid value.<br/>  WARNING: the
	 * array order is not related to actual IntGrid values! As user can re-order IntGrid values
	 * freely, you may value "2" before value "1" in this array.
	 */
	[[nodiscard]] const std::vector<IntGridValueDefinition> &get_int_grid_values() const { return int_grid_values; }

	std::vector<IntGridValueDefinition> &get_mutable_int_grid_values() { return int_grid_values; }

	void set_int_grid_values(const std::vector<IntGridValueDefinition> &value) { this->int_grid_values = value; }

	/**
	 * Group information for IntGrid values
	 */
	[[nodiscard]] const std::vector<IntGridValueGroupDefinition> &get_int_grid_values_groups() const { return int_grid_values_groups; }

	std::vector<IntGridValueGroupDefinition> &get_mutable_int_grid_values_groups() { return int_grid_values_groups; }

	void set_int_grid_values_groups(const std::vector<IntGridValueGroupDefinition> &value) { this->int_grid_values_groups = value; }

	/**
	 * Parallax horizontal factor (from -1 to 1, defaults to 0) which affects the scrolling
	 * speed of this layer, creating a fake 3D (parallax) effect.
	 */
	[[nodiscard]] const double &get_parallax_factor_x() const { return parallax_factor_x; }

	double &get_mutable_parallax_factor_x() { return parallax_factor_x; }

	void set_parallax_factor_x(const double &value) { this->parallax_factor_x = value; }

	/**
	 * Parallax vertical factor (from -1 to 1, defaults to 0) which affects the scrolling speed
	 * of this layer, creating a fake 3D (parallax) effect.
	 */
	[[nodiscard]] const double &get_parallax_factor_y() const { return parallax_factor_y; }

	double &get_mutable_parallax_factor_y() { return parallax_factor_y; }

	void set_parallax_factor_y(const double &value) { this->parallax_factor_y = value; }

	/**
	 * If true (default), a layer with a parallax factor will also be scaled up/down accordingly.
	 */
	[[nodiscard]] const bool &get_parallax_scaling() const { return parallax_scaling; }

	bool &get_mutable_parallax_scaling() { return parallax_scaling; }

	void set_parallax_scaling(const bool &value) { this->parallax_scaling = value; }

	/**
	 * X offset of the layer, in pixels (IMPORTANT: this should be added to the `LdtkLayer`
	 * optional offset)
	 */
	[[nodiscard]] const int64_t &get_px_offset_x() const { return px_offset_x; }

	int64_t &get_mutable_px_offset_x() { return px_offset_x; }

	void set_px_offset_x(const int64_t &value) { this->px_offset_x = value; }

	/**
	 * Y offset of the layer, in pixels (IMPORTANT: this should be added to the `LdtkLayer`
	 * optional offset)
	 */
	[[nodiscard]] const int64_t &get_px_offset_y() const { return px_offset_y; }

	int64_t &get_mutable_px_offset_y() { return px_offset_y; }

	void set_px_offset_y(const int64_t &value) { this->px_offset_y = value; }

	/**
	 * If TRUE, the content of this layer will be used when rendering levels in a simplified way
	 * for the world view
	 */
	[[nodiscard]] const bool &get_render_in_world_view() const { return render_in_world_view; }

	bool &get_mutable_render_in_world_view() { return render_in_world_view; }

	void set_render_in_world_view(const bool &value) { this->render_in_world_view = value; }

	/**
	 * An array of tags to filter Entities that can be added to this layer
	 */
	[[nodiscard]] const StringVec &get_required_tags() const { return required_tags; }

	StringVec &get_mutable_required_tags() { return required_tags; }

	void set_required_tags(const StringVec &value) { this->required_tags = value; }

	/**
	 * If the tiles are smaller or larger than the layer grid, the pivot value will be used to
	 * position the tile relatively its grid cell.
	 */
	[[nodiscard]] const double &get_tile_pivot_x() const { return tile_pivot_x; }

	double &get_mutable_tile_pivot_x() { return tile_pivot_x; }

	void set_tile_pivot_x(const double &value) { this->tile_pivot_x = value; }

	/**
	 * If the tiles are smaller or larger than the layer grid, the pivot value will be used to
	 * position the tile relatively its grid cell.
	 */
	[[nodiscard]] const double &get_tile_pivot_y() const { return tile_pivot_y; }

	double &get_mutable_tile_pivot_y() { return tile_pivot_y; }

	void set_tile_pivot_y(const double &value) { this->tile_pivot_y = value; }

	/**
	 * Reference to the default Tileset UID being used by this layer definition.<br/>
	 * **WARNING**: some layer *instances* might use a different tileset. So most of the time,
	 * you should probably use the `__tilesetDefUid` value found in layer instances.<br/>  Note:
	 * since version 1.0.0, the old `autoTilesetDefUid` was removed and merged into this value.
	 */
	[[nodiscard]] optional<int64_t> get_tileset_def_uid() const { return tileset_def_uid; }

	void set_tileset_def_uid(optional<int64_t> value) { this->tileset_def_uid = value; }

	/**
	 * Type of the layer as Haxe Enum Possible values: `IntGrid`, `Entities`, `Tiles`,
	 * `AutoLayer`
	 */
	[[nodiscard]] const Type &get_layer_definition_type() const { return layer_definition_type; }

	Type &get_mutable_layer_definition_type() { return layer_definition_type; }

	void set_layer_definition_type(const Type &value) { this->layer_definition_type = value; }

	/**
	 * User defined color for the UI
	 */
	[[nodiscard]] optional<string> get_ui_color() const { return ui_color; }

	void set_ui_color(optional<string> value) { this->ui_color = std::move(value); }

	/**
	 * Unique Int identifier
	 */
	[[nodiscard]] const int64_t &get_uid() const { return uid; }

	int64_t &get_mutable_uid() { return uid; }

	void set_uid(const int64_t &value) { this->uid = value; }

	/**
	 * Display tags
	 */
	[[nodiscard]] const StringVec &get_ui_filter_tags() const { return ui_filter_tags; }

	StringVec &get_mutable_ui_filter_tags() { return ui_filter_tags; }

	void set_ui_filter_tags(const StringVec &value) { this->ui_filter_tags = value; }

	/**
	 * Asynchronous rendering option for large/complex layers
	 */
	[[nodiscard]] const bool &get_use_async_render() const { return use_async_render; }

	bool &get_mutable_use_async_render() { return use_async_render; }

	void set_use_async_render(const bool &value) { this->use_async_render = value; }
};

/**
 * In a tileset definition, user defined meta-data of a tile.
 */
class WVK_API TileCustomMetadata
{
  public:
	TileCustomMetadata()          = default;
	virtual ~TileCustomMetadata() = default;

  private:
	string  data;
	int64_t tile_id{};

  public:
	[[nodiscard]] const string &get_data() const { return data; }

	string &get_mutable_data() { return data; }

	void set_data(const string &value) { this->data = value; }

	[[nodiscard]] const int64_t &get_tile_id() const { return tile_id; }

	int64_t &get_mutable_tile_id() { return tile_id; }

	void set_tile_id(const int64_t &value) { this->tile_id = value; }
};

enum class EmbedAtlas : int
{
	LDTK_ICONS
};

/**
 * In a tileset definition, enum based tag infos
 */
class WVK_API EnumTagValue
{
  public:
	EnumTagValue()          = default;
	virtual ~EnumTagValue() = default;

  private:
	string               enum_value_id;
	std::vector<int64_t> tile_ids;

  public:
	[[nodiscard]] const string &get_enum_value_id() const { return enum_value_id; }
	string                     &get_mutable_enum_value_id() { return enum_value_id; }
	void                        set_enum_value_id(const string &value) { this->enum_value_id = value; }

	[[nodiscard]] const std::vector<int64_t> &get_tile_ids() const { return tile_ids; }
	std::vector<int64_t>                     &get_mutable_tile_ids() { return tile_ids; }
	void                                      set_tile_ids(const std::vector<int64_t> &value) { this->tile_ids = value; }
};

/**
 * The Tileset definition is the most important part among project definitions. It
 * contains some extra information about each integrated tileset. If you only had to parse
 * one definition section, that would be the one.
 */
class WVK_API TilesetDefinition
{
  public:
	TilesetDefinition()          = default;
	virtual ~TilesetDefinition() = default;

  private:
	int64_t                             c_hei{};
	int64_t                             c_wid{};
	optional<std::map<string, json>>    cached_pixel_data;
	std::vector<TileCustomMetadata>     custom_data;
	optional<EmbedAtlas>                embed_atlas;
	std::vector<EnumTagValue>           enum_tags;
	string                              identifier;
	int64_t                             padding{};
	int64_t                             px_hei{};
	int64_t                             px_wid{};
	optional<string>                    rel_path;
	std::vector<std::map<string, json>> saved_selections;
	int64_t                             spacing{};
	StringVec                           tags;
	optional<int64_t>                   tags_source_enum_uid;
	int64_t                             tile_grid_size{};
	int64_t                             uid{};

  public:
	/**
	 * Grid-based height
	 */
	[[nodiscard]] const int64_t &get_c_hei() const { return c_hei; }

	int64_t &get_mutable_c_hei() { return c_hei; }

	void set_c_hei(const int64_t &value) { this->c_hei = value; }

	/**
	 * Grid-based width
	 */
	[[nodiscard]] const int64_t &get_c_wid() const { return c_wid; }

	int64_t &get_mutable_c_wid() { return c_wid; }

	void set_c_wid(const int64_t &value) { this->c_wid = value; }

	/**
	 * The following data is used internally for various optimizations. It's always synced with
	 * source image changes.
	 */
	[[nodiscard]] optional<std::map<string, json>> get_cached_pixel_data() const { return cached_pixel_data; }

	void set_cached_pixel_data(optional<std::map<string, json>> value) { this->cached_pixel_data = std::move(value); }

	/**
	 * An array of custom tile metadata
	 */
	[[nodiscard]] const std::vector<TileCustomMetadata> &get_custom_data() const { return custom_data; }

	std::vector<TileCustomMetadata> &get_mutable_custom_data() { return custom_data; }

	void set_custom_data(const std::vector<TileCustomMetadata> &value) { this->custom_data = value; }

	/**
	 * If this value is set, then it means that this atlas uses an internal LDtk atlas image
	 * instead of a loaded one. Possible values: &lt;`null`&gt;, `LdtkIcons`
	 */
	[[nodiscard]] optional<EmbedAtlas> get_embed_atlas() const { return embed_atlas; }

	void set_embed_atlas(optional<EmbedAtlas> value) { this->embed_atlas = value; }

	/**
	 * Tileset tags using Enum values specified by `tagsSourceEnumId`. This array contains 1
	 * element per Enum value, which contains an array of all Tile IDs that are tagged with it.
	 */
	[[nodiscard]] const std::vector<EnumTagValue> &get_enum_tags() const { return enum_tags; }

	std::vector<EnumTagValue> &get_mutable_enum_tags() { return enum_tags; }

	void set_enum_tags(const std::vector<EnumTagValue> &value) { this->enum_tags = value; }

	/**
	 * User defined unique identifier
	 */
	[[nodiscard]] const string &get_identifier() const { return identifier; }

	string &get_mutable_identifier() { return identifier; }

	void set_identifier(const string &value) { this->identifier = value; }

	/**
	 * Distance in pixels from image borders
	 */
	[[nodiscard]] const int64_t &get_padding() const { return padding; }

	int64_t &get_mutable_padding() { return padding; }

	void set_padding(const int64_t &value) { this->padding = value; }

	/**
	 * Image height in pixels
	 */
	[[nodiscard]] const int64_t &get_px_hei() const { return px_hei; }

	int64_t &get_mutable_px_hei() { return px_hei; }

	void set_px_hei(const int64_t &value) { this->px_hei = value; }

	/**
	 * Image width in pixels
	 */
	[[nodiscard]] const int64_t &get_px_wid() const { return px_wid; }

	int64_t &get_mutable_px_wid() { return px_wid; }

	void set_px_wid(const int64_t &value) { this->px_wid = value; }

	/**
	 * Path to the source file, relative to the current project JSON file<br/>  It can be null
	 * if no image was provided, or when using an embed atlas.
	 */
	[[nodiscard]] optional<string> get_rel_path() const { return rel_path; }

	void set_rel_path(optional<string> value) { this->rel_path = std::move(value); }

	/**
	 * Array of group of tiles selections, only meant to be used in the editor
	 */
	[[nodiscard]] const std::vector<std::map<string, json>> &get_saved_selections() const { return saved_selections; }

	std::vector<std::map<string, json>> &get_mutable_saved_selections() { return saved_selections; }

	void set_saved_selections(const std::vector<std::map<string, json>> &value) { this->saved_selections = value; }

	/**
	 * Space in pixels between all tiles
	 */
	[[nodiscard]] const int64_t &get_spacing() const { return spacing; }

	int64_t &get_mutable_spacing() { return spacing; }

	void set_spacing(const int64_t &value) { this->spacing = value; }

	/**
	 * An array of user-defined tags to organize the Tilesets
	 */
	[[nodiscard]] const StringVec &get_tags() const { return tags; }

	StringVec &get_mutable_tags() { return tags; }

	void set_tags(const StringVec &value) { this->tags = value; }

	/**
	 * Optional Enum definition UID used for this tileset meta-data
	 */
	[[nodiscard]] optional<int64_t> get_tags_source_enum_uid() const { return tags_source_enum_uid; }

	void set_tags_source_enum_uid(optional<int64_t> value) { this->tags_source_enum_uid = value; }

	[[nodiscard]] const int64_t &get_tile_grid_size() const { return tile_grid_size; }

	int64_t &get_mutable_tile_grid_size() { return tile_grid_size; }

	void set_tile_grid_size(const int64_t &value) { this->tile_grid_size = value; }

	/**
	 * Unique Identifier
	 */
	[[nodiscard]] const int64_t &get_uid() const { return uid; }

	int64_t &get_mutable_uid() { return uid; }

	void set_uid(const int64_t &value) { this->uid = value; }
};

/**
 * If you're writing your own LDtk importer, you should probably just ignore *most* stuff in
 * the `defs` section, as it contains data that are mostly important to the editor. To keep
 * you away from the `defs` section and avoid some unnecessary JSON parsing, important data
 * from definitions is often duplicated in fields prefixed with a double underscore (eg.
 * `__identifier` or `__type`).  The 2 only definition types you might need here are
 * **Tilesets** and **Enums**.
 *
 * A structure containing all the definitions of this project
 */
class WVK_API LdtkDefs
{
  public:
	LdtkDefs()          = default;
	virtual ~LdtkDefs() = default;

  private:
	std::vector<EntityDefinition>  entities;
	std::vector<EnumDefinition>    enums;
	std::vector<EnumDefinition>    external_enums;
	std::vector<LayerDefinition>   layers;
	std::vector<FieldDefinition>   level_fields;
	std::vector<TilesetDefinition> tilesets;

  public:
	/**
	 * All entities definitions, including their custom fields
	 */
	[[nodiscard]] const std::vector<EntityDefinition> &get_entities() const { return entities; }

	std::vector<EntityDefinition> &get_mutable_entities() { return entities; }

	void set_entities(const std::vector<EntityDefinition> &value) { this->entities = value; }

	/**
	 * All internal enums
	 */
	[[nodiscard]] const std::vector<EnumDefinition> &get_enums() const { return enums; }

	std::vector<EnumDefinition> &get_mutable_enums() { return enums; }

	void set_enums(const std::vector<EnumDefinition> &value) { this->enums = value; }

	/**
	 * Note: external enums are exactly the same as `enums`, except they have a `relPath` to
	 * point to an external source file.
	 */
	[[nodiscard]] const std::vector<EnumDefinition> &get_external_enums() const { return external_enums; }

	std::vector<EnumDefinition> &get_mutable_external_enums() { return external_enums; }

	void set_external_enums(const std::vector<EnumDefinition> &value) { this->external_enums = value; }

	/**
	 * All layer definitions
	 */
	[[nodiscard]] const std::vector<LayerDefinition> &get_layers() const { return layers; }

	std::vector<LayerDefinition> &get_mutable_layers() { return layers; }

	void set_layers(const std::vector<LayerDefinition> &value) { this->layers = value; }

	/**
	 * All custom fields available to all levels.
	 */
	[[nodiscard]] const std::vector<FieldDefinition> &get_level_fields() const { return level_fields; }

	std::vector<FieldDefinition> &get_mutable_level_fields() { return level_fields; }

	void set_level_fields(const std::vector<FieldDefinition> &value) { this->level_fields = value; }

	/**
	 * All tilesets
	 */
	[[nodiscard]] const std::vector<TilesetDefinition> &get_tilesets() const { return tilesets; }

	std::vector<TilesetDefinition> &get_mutable_tilesets() { return tilesets; }

	void set_tilesets(const std::vector<TilesetDefinition> &value) { this->tilesets = value; }
};

enum class Flag : int
{
	DISCARD_PRE_CSV_INT_GRID,
	EXPORT_OLD_TABLE_OF_CONTENT_DATA,
	EXPORT_PRE_CSV_INT_GRID_FORMAT,
	IGNORE_BACKUP_SUGGEST,
	MULTI_WORLDS,
	PREPEND_INDEX_TO_LEVEL_FILE_NAMES,
	USE_MULTILINES_TYPE
};

class WVK_API FieldInstance
{
  public:
	FieldInstance()          = default;
	virtual ~FieldInstance() = default;

  private:
	string                     identifier;
	optional<TilesetRectangle> tile;
	string                     type;
	json                       value;
	int64_t                    def_uid{};
	std::vector<json>          real_editor_values;

  public:
	/**
	 * Field definition identifier
	 */
	[[nodiscard]] const string &get_identifier() const { return identifier; }

	string &get_mutable_identifier() { return identifier; }

	void set_identifier(const string &v) { this->identifier = v; }

	/**
	 * Optional TilesetRect used to display this field (this can be the field own Tile, or some
	 * other Tile guessed from the value, like an Enum).
	 */
	[[nodiscard]] optional<TilesetRectangle> get_tile() const { return tile; }

	void set_tile(optional<TilesetRectangle> v) { this->tile = std::move(v); }

	/**
	 * Type of the field, such as `Int`, `Float`, `String`, `Enum(my_enum_name)`, `Bool`,
	 * etc.<br/>  NOTE: if you enable the advanced option **Use Multilines type**, you will have
	 * "*Multilines*" instead of "*String*" when relevant.
	 */
	[[nodiscard]] const string &get_type() const { return type; }

	string &get_mutable_type() { return type; }

	void set_type(const string &v) { this->type = v; }

	/**
	 * Actual value of the field instance. The value type varies, depending on `__type`:<br/>
	 * - For **class WVK_API ic types** (ie. Integer, Float, Boolean, String, Text and FilePath), you
	 * just get the actual value with the expected type.<br/>   - For **Color**, the value is an
	 * hexadecimal string using "#rrggbb" format.<br/>   - For **Enum**, the value is a String
	 * representing the selected enum value.<br/>   - For **Point**, the value is a
	 * [GridPoint](#ldtk-GridPoint) object.<br/>   - For **Tile**, the value is a
	 * [TilesetRect](#ldtk-TilesetRect) object.<br/>   - For **EntityRef**, the value is an
	 * [EntityReferenceInfos](#ldtk-EntityReferenceInfos) object.<br/><br/>  If the field is an
	 * array, then this `__value` will also be a JSON array.
	 */
	[[nodiscard]] const json &get_value() const { return value; }

	json &get_mutable_value() { return value; }

	void set_value(const json &v) { this->value = v; }

	/**
	 * Reference of the **Field definition** UID
	 */
	[[nodiscard]] const int64_t &get_def_uid() const { return def_uid; }

	int64_t &get_mutable_def_uid() { return def_uid; }

	void set_def_uid(const int64_t &v) { this->def_uid = v; }

	/**
	 * Editor internal raw values
	 */
	[[nodiscard]] const std::vector<json> &get_real_editor_values() const { return real_editor_values; }

	std::vector<json> &get_mutable_real_editor_values() { return real_editor_values; }

	void set_real_editor_values(const std::vector<json> &v) { this->real_editor_values = v; }
};

class WVK_API EntityInstance
{
  public:
	EntityInstance()          = default;
	virtual ~EntityInstance() = default;

  private:
	std::vector<int64_t>       grid;
	string                     identifier;
	std::vector<double>        pivot;
	string                     smart_color;
	StringVec                  tags;
	optional<TilesetRectangle> tile;
	optional<int64_t>          world_x;
	optional<int64_t>          world_y;
	int64_t                    def_uid{};
	std::vector<FieldInstance> field_instances;
	int64_t                    height{};
	string                     iid;
	std::vector<int64_t>       px;
	int64_t                    width{};

  public:
	/**
	 * Grid-based coordinates (`[x,y]` format)
	 */
	[[nodiscard]] const std::vector<int64_t> &get_grid() const { return grid; }

	std::vector<int64_t> &get_mutable_grid() { return grid; }

	void set_grid(const std::vector<int64_t> &value) { this->grid = value; }

	/**
	 * Entity definition identifier
	 */
	[[nodiscard]] const string &get_identifier() const { return identifier; }

	string &get_mutable_identifier() { return identifier; }

	void set_identifier(const string &value) { this->identifier = value; }

	/**
	 * Pivot coordinates  (`[x,y]` format, values are from 0 to 1) of the Entity
	 */
	[[nodiscard]] const std::vector<double> &get_pivot() const { return pivot; }

	std::vector<double> &get_mutable_pivot() { return pivot; }

	void set_pivot(const std::vector<double> &value) { this->pivot = value; }

	/**
	 * The entity "smart" color, guessed from either Entity definition, or one its field
	 * instances.
	 */
	[[nodiscard]] const string &get_smart_color() const { return smart_color; }

	string &get_mutable_smart_color() { return smart_color; }

	void set_smart_color(const string &value) { this->smart_color = value; }

	/**
	 * Array of tags defined in this Entity definition
	 */
	[[nodiscard]] const StringVec &get_tags() const { return tags; }

	StringVec &get_mutable_tags() { return tags; }

	void set_tags(const StringVec &value) { this->tags = value; }

	/**
	 * Optional TilesetRect used to display this entity (it could either be the default Entity
	 * tile, or some tile provided by a field value, like an Enum).
	 */
	[[nodiscard]] optional<TilesetRectangle> get_tile() const { return tile; }

	void set_tile(optional<TilesetRectangle> value) { this->tile = std::move(value); }

	/**
	 * X world coordinate in pixels. Only available in GridVania or Free world layouts.
	 */
	[[nodiscard]] optional<int64_t> get_world_x() const { return world_x; }

	void set_world_x(optional<int64_t> value) { this->world_x = value; }

	/**
	 * Y world coordinate in pixels Only available in GridVania or Free world layouts.
	 */
	[[nodiscard]] optional<int64_t> get_world_y() const { return world_y; }

	void set_world_y(optional<int64_t> value) { this->world_y = value; }

	/**
	 * Reference of the **Entity definition** UID
	 */
	[[nodiscard]] const int64_t &get_def_uid() const { return def_uid; }

	int64_t &get_mutable_def_uid() { return def_uid; }

	void set_def_uid(const int64_t &value) { this->def_uid = value; }

	/**
	 * An array of all custom fields and their values.
	 */
	[[nodiscard]] const std::vector<FieldInstance> &get_field_instances() const { return field_instances; }

	std::vector<FieldInstance> &get_mutable_field_instances() { return field_instances; }

	void set_field_instances(const std::vector<FieldInstance> &value) { this->field_instances = value; }

	/**
	 * Entity height in pixels. For non-resizable entities, it will be the same as Entity
	 * definition.
	 */
	[[nodiscard]] const int64_t &get_height() const { return height; }

	int64_t &get_mutable_height() { return height; }

	void set_height(const int64_t &value) { this->height = value; }

	/**
	 * Unique instance identifier
	 */
	[[nodiscard]] const string &get_iid() const { return iid; }

	string &get_mutable_iid() { return iid; }

	void set_iid(const string &value) { this->iid = value; }

	/**
	 * Pixel coordinates (`[x,y]` format) in current level coordinate space. Don't forget
	 * optional layer offsets, if they exist!
	 */
	[[nodiscard]] const std::vector<int64_t> &get_px() const { return px; }

	std::vector<int64_t> &get_mutable_px() { return px; }

	void set_px(const std::vector<int64_t> &value) { this->px = value; }

	/**
	 * Entity width in pixels. For non-resizable entities, it will be the same as Entity
	 * definition.
	 */
	[[nodiscard]] const int64_t &get_width() const { return width; }

	int64_t &get_mutable_width() { return width; }

	void set_width(const int64_t &value) { this->width = value; }
};

/**
 * This object describes the "location" of an Entity instance in the project worlds.
 *
 * IID information of this instance
 */
class WVK_API ReferenceToAnEntityInstance
{
  public:
	ReferenceToAnEntityInstance()          = default;
	virtual ~ReferenceToAnEntityInstance() = default;

  private:
	string entity_iid;
	string layer_iid;
	string level_iid;
	string world_iid;

  public:
	/**
	 * IID of the referred EntityInstance
	 */
	[[nodiscard]] const string &get_entity_iid() const { return entity_iid; }

	string &get_mutable_entity_iid() { return entity_iid; }

	void set_entity_iid(const string &value) { this->entity_iid = value; }

	/**
	 * IID of the LdtkLayer containing the referred EntityInstance
	 */
	[[nodiscard]] const string &get_layer_iid() const { return layer_iid; }

	string &get_mutable_layer_iid() { return layer_iid; }

	void set_layer_iid(const string &value) { this->layer_iid = value; }

	/**
	 * IID of the Level containing the referred EntityInstance
	 */
	[[nodiscard]] const string &get_level_iid() const { return level_iid; }

	string &get_mutable_level_iid() { return level_iid; }

	void set_level_iid(const string &value) { this->level_iid = value; }

	/**
	 * IID of the World containing the referred EntityInstance
	 */
	[[nodiscard]] const string &get_world_iid() const { return world_iid; }

	string &get_mutable_world_iid() { return world_iid; }

	void set_world_iid(const string &value) { this->world_iid = value; }
};

/**
 * This object is just a grid-based coordinate used in Field values.
 */
class WVK_API GridPoint
{
  public:
	GridPoint()          = default;
	virtual ~GridPoint() = default;

  private:
	int64_t cx{};
	int64_t cy{};

  public:
	/**
	 * X grid-based coordinate
	 */
	[[nodiscard]] const int64_t &get_cx() const { return cx; }

	int64_t &get_mutable_cx() { return cx; }

	void set_cx(const int64_t &value) { this->cx = value; }

	/**
	 * Y grid-based coordinate
	 */
	[[nodiscard]] const int64_t &get_cy() const { return cy; }

	int64_t &get_mutable_cy() { return cy; }

	void set_cy(const int64_t &value) { this->cy = value; }
};

/**
 * IntGrid value instance
 */
class WVK_API IntGridValueInstance
{
  public:
	IntGridValueInstance()          = default;
	virtual ~IntGridValueInstance() = default;

  private:
	int64_t coord_id{};
	int64_t v{};

  public:
	/**
	 * Coordinate ID in the layer grid
	 */
	[[nodiscard]] const int64_t &get_coord_id() const { return coord_id; }

	int64_t &get_mutable_coord_id() { return coord_id; }

	void set_coord_id(const int64_t &value) { this->coord_id = value; }

	/**
	 * IntGrid value
	 */
	[[nodiscard]] const int64_t &get_v() const { return v; }

	int64_t &get_mutable_v() { return v; }

	void set_v(const int64_t &value) { this->v = value; }
};

/**
 * This structure represents a single tile from a given Tileset.
 */
class WVK_API TileInstance
{
  public:
	TileInstance()          = default;
	virtual ~TileInstance() = default;

  private:
	double               a{};
	std::vector<int64_t> d;
	int64_t              f{};
	std::vector<int64_t> px;
	std::vector<int64_t> src;
	int64_t              t{};

  public:
	/**
	 * Alpha/opacity of the tile (0-1, defaults to 1)
	 */
	[[nodiscard]] const double &get_a() const { return a; }

	double &get_mutable_a() { return a; }

	void set_a(const double &value) { this->a = value; }

	/**
	 * Internal data used by the editor.
	 * For auto-layer tiles: `[ruleId, coordId]`.
	 * For tile-layer tiles: `[coordId]`.
	 */
	[[nodiscard]] const std::vector<int64_t> &get_d() const { return d; }

	std::vector<int64_t> &get_mutable_d() { return d; }

	void set_d(const std::vector<int64_t> &value) { this->d = value; }

	/**
	 * "Flip bits", a 2-bits integer to represent the mirror transformations of the tile.
	 * - Bit 0 = X flip
	 * - Bit 1 = Y flip
	 * Examples: f=0 (no flip), f=1 (X flip only), f=2 (Y flip only), f=3 (both flips)
	 */
	[[nodiscard]] const int64_t &get_f() const { return f; }

	int64_t &get_mutable_f() { return f; }

	void set_f(const int64_t &value) { this->f = value; }

	/**
	 * Pixel coordinates of the tile in the layer (`[x,y]` format).
	 * Don't forget optional layer offsets, if they exist!
	 */
	[[nodiscard]] const std::vector<int64_t> &get_px() const { return px; }

	std::vector<int64_t> &get_mutable_px() { return px; }

	void set_px(const std::vector<int64_t> &value) { this->px = value; }

	/**
	 * Pixel coordinates of the tile in the tileset (`[x,y]` format)
	 */
	[[nodiscard]] const std::vector<int64_t> &get_src() const { return src; }

	std::vector<int64_t> &get_mutable_src() { return src; }

	void set_src(const std::vector<int64_t> &value) { this->src = value; }

	/**
	 * The *Tile ID* in the corresponding tileset.
	 */
	[[nodiscard]] const int64_t &get_t() const { return t; }

	int64_t &get_mutable_t() { return t; }

	void set_t(const int64_t &value) { this->t = value; }
};

class WVK_API LdtkLayer
{
  public:
	LdtkLayer()          = default;
	virtual ~LdtkLayer() = default;

  private:
	int64_t                                     c_hei{};
	int64_t                                     c_wid{};
	int64_t                                     grid_size{};
	string                                      identifier;
	double                                      opacity{};
	int64_t                                     px_total_offset_x{};
	int64_t                                     px_total_offset_y{};
	optional<int64_t>                           tileset_def_uid;
	optional<string>                            tileset_rel_path;
	string                                      type;
	std::vector<TileInstance>                   auto_layer_tiles;
	std::vector<EntityInstance>                 entity_instances;
	std::vector<TileInstance>                   grid_tiles;
	string                                      iid;
	optional<std::vector<IntGridValueInstance>> int_grid;
	std::vector<int64_t>                        int_grid_csv;
	int64_t                                     layer_def_uid{};
	int64_t                                     level_id{};
	std::vector<int64_t>                        optional_rules;
	optional<int64_t>                           override_tileset_uid;
	int64_t                                     px_offset_x{};
	int64_t                                     px_offset_y{};
	int64_t                                     seed{};
	bool                                        visible{};

  public:
	/**
	 * Grid-based height
	 */
	[[nodiscard]] const int64_t &get_c_hei() const { return c_hei; }

	int64_t &get_mutable_c_hei() { return c_hei; }

	void set_c_hei(const int64_t &value) { this->c_hei = value; }

	/**
	 * Grid-based width
	 */
	[[nodiscard]] const int64_t &get_c_wid() const { return c_wid; }

	int64_t &get_mutable_c_wid() { return c_wid; }

	void set_c_wid(const int64_t &value) { this->c_wid = value; }

	/**
	 * Grid size
	 */
	[[nodiscard]] const int64_t &get_grid_size() const { return grid_size; }

	int64_t &get_mutable_grid_size() { return grid_size; }

	void set_grid_size(const int64_t &value) { this->grid_size = value; }

	/**
	 * Layer definition identifier
	 */
	[[nodiscard]] const string &get_identifier() const { return identifier; }

	string &get_mutable_identifier() { return identifier; }

	void set_identifier(const string &value) { this->identifier = value; }

	/**
	 * Layer opacity as Float [0-1]
	 */
	[[nodiscard]] const double &get_opacity() const { return opacity; }

	double &get_mutable_opacity() { return opacity; }

	void set_opacity(const double &value) { this->opacity = value; }

	/**
	 * Total layer X pixel offset, including both instance and definition offsets.
	 */
	[[nodiscard]] const int64_t &get_px_total_offset_x() const { return px_total_offset_x; }

	int64_t &get_mutable_px_total_offset_x() { return px_total_offset_x; }

	void set_px_total_offset_x(const int64_t &value) { this->px_total_offset_x = value; }

	/**
	 * Total layer Y pixel offset, including both instance and definition offsets.
	 */
	[[nodiscard]] const int64_t &get_px_total_offset_y() const { return px_total_offset_y; }

	int64_t &get_mutable_px_total_offset_y() { return px_total_offset_y; }

	void set_px_total_offset_y(const int64_t &value) { this->px_total_offset_y = value; }

	/**
	 * The definition UID of corresponding Tileset, if any.
	 */
	[[nodiscard]] optional<int64_t> get_tileset_def_uid() const { return tileset_def_uid; }

	void set_tileset_def_uid(optional<int64_t> value) { this->tileset_def_uid = value; }

	/**
	 * The relative path to corresponding Tileset, if any.
	 */
	[[nodiscard]] optional<string> get_tileset_rel_path() const { return tileset_rel_path; }

	void set_tileset_rel_path(optional<string> value) { this->tileset_rel_path = std::move(value); }

	/**
	 * Layer type (possible values: IntGrid, Entities, Tiles or AutoLayer)
	 */
	[[nodiscard]] const string &get_type() const { return type; }

	string &get_mutable_type() { return type; }

	void set_type(const string &value) { this->type = value; }

	/**
	 * An array containing all tiles generated by Auto-layer rules. The array is already sorted
	 * in display order (ie. 1st tile is beneath 2nd, which is beneath 3rd etc.).
	 * Note: if multiple tiles are stacked in the same cell as the result of different rules,
	 * all tiles behind opaque ones will be discarded.
	 */
	[[nodiscard]] const std::vector<TileInstance> &get_auto_layer_tiles() const { return auto_layer_tiles; }

	std::vector<TileInstance> &get_mutable_auto_layer_tiles() { return auto_layer_tiles; }

	void set_auto_layer_tiles(const std::vector<TileInstance> &value) { this->auto_layer_tiles = value; }

	[[nodiscard]] const std::vector<EntityInstance> &get_entity_instances() const { return entity_instances; }

	std::vector<EntityInstance> &get_mutable_entity_instances() { return entity_instances; }

	void set_entity_instances(const std::vector<EntityInstance> &value) { this->entity_instances = value; }

	[[nodiscard]] const std::vector<TileInstance> &get_grid_tiles() const { return grid_tiles; }

	std::vector<TileInstance> &get_mutable_grid_tiles() { return grid_tiles; }

	void set_grid_tiles(const std::vector<TileInstance> &value) { this->grid_tiles = value; }

	/**
	 * Unique layer instance identifier
	 */
	[[nodiscard]] const string &get_iid() const { return iid; }

	string &get_mutable_iid() { return iid; }

	void set_iid(const string &value) { this->iid = value; }

	/**
	 * **WARNING**: this deprecated value is no longer exported since version 1.0.0  Replaced
	 * by: `intGridCsv`
	 */
	[[nodiscard]] optional<std::vector<IntGridValueInstance>> get_int_grid() const { return int_grid; }

	void set_int_grid(optional<std::vector<IntGridValueInstance>> value) { this->int_grid = std::move(value); }

	/**
	 * A list of all values in the IntGrid layer, stored in CSV format (Comma Separated
	 * Values).<br/>  Order is from left to right, and top to bottom (ie. first row from left to
	 * right, followed by second row, etc).<br/>  `0` means "empty cell" and IntGrid values
	 * start at 1.<br/>  The array size is `__cWid` x `__cHei` cells.
	 */
	[[nodiscard]] const std::vector<int64_t> &get_int_grid_csv() const { return int_grid_csv; }

	std::vector<int64_t> &get_mutable_int_grid_csv() { return int_grid_csv; }

	void set_int_grid_csv(const std::vector<int64_t> &value) { this->int_grid_csv = value; }

	/**
	 * Reference the Layer definition UID
	 */
	[[nodiscard]] const int64_t &get_layer_def_uid() const { return layer_def_uid; }

	int64_t &get_mutable_layer_def_uid() { return layer_def_uid; }

	void set_layer_def_uid(const int64_t &value) { this->layer_def_uid = value; }

	/**
	 * Reference to the UID of the level containing this layer instance
	 */
	[[nodiscard]] const int64_t &get_level_id() const { return level_id; }

	int64_t &get_mutable_level_id() { return level_id; }

	void set_level_id(const int64_t &value) { this->level_id = value; }

	/**
	 * An Array containing the UIDs of optional rules that were enabled in this specific layer
	 * instance.
	 */
	[[nodiscard]] const std::vector<int64_t> &get_optional_rules() const { return optional_rules; }

	std::vector<int64_t> &get_mutable_optional_rules() { return optional_rules; }

	void set_optional_rules(const std::vector<int64_t> &value) { this->optional_rules = value; }

	/**
	 * This layer can use another tileset by overriding the tileset UID here.
	 */
	[[nodiscard]] optional<int64_t> get_override_tileset_uid() const { return override_tileset_uid; }

	void set_override_tileset_uid(optional<int64_t> value) { this->override_tileset_uid = value; }

	/**
	 * X offset in pixels to render this layer, usually 0 (IMPORTANT: this should be added to
	 * the `LayerDef` optional offset, so you should probably prefer using `__pxTotalOffsetX`
	 * which contains the total offset value)
	 */
	[[nodiscard]] const int64_t &get_px_offset_x() const { return px_offset_x; }

	int64_t &get_mutable_px_offset_x() { return px_offset_x; }

	void set_px_offset_x(const int64_t &value) { this->px_offset_x = value; }

	/**
	 * Y offset in pixels to render this layer, usually 0 (IMPORTANT: this should be added to
	 * the `LayerDef` optional offset, so you should probably prefer using `__pxTotalOffsetX`
	 * which contains the total offset value)
	 */
	[[nodiscard]] const int64_t &get_px_offset_y() const { return px_offset_y; }

	int64_t &get_mutable_px_offset_y() { return px_offset_y; }

	void set_px_offset_y(const int64_t &value) { this->px_offset_y = value; }

	/**
	 * Random seed used for Auto-Layers rendering
	 */
	[[nodiscard]] const int64_t &get_seed() const { return seed; }

	int64_t &get_mutable_seed() { return seed; }

	void set_seed(const int64_t &value) { this->seed = value; }

	/**
	 * Layer instance visibility
	 */
	[[nodiscard]] const bool &get_visible() const { return visible; }

	bool &get_mutable_visible() { return visible; }

	void set_visible(const bool &value) { this->visible = value; }
};

/**
 * Level background image position info
 */
class WVK_API LevelBackgroundPosition
{
  public:
	LevelBackgroundPosition()          = default;
	virtual ~LevelBackgroundPosition() = default;

  private:
	std::vector<double>  crop_rect;
	std::vector<double>  scale;
	std::vector<int64_t> top_left_px;

  public:
	/**
	 * An array of 4 float values describing the cropped sub-rectangle of the displayed
	 * background image. This cropping happens when original is larger than the level bounds.
	 * Array format: `[ cropX, cropY, cropWidth, cropHeight ]`
	 */
	[[nodiscard]] const std::vector<double> &get_crop_rect() const { return crop_rect; }

	std::vector<double> &get_mutable_crop_rect() { return crop_rect; }

	void set_crop_rect(const std::vector<double> &value) { this->crop_rect = value; }

	/**
	 * An array containing the `[scaleX,scaleY]` values of the **cropped** background image,
	 * depending on `bgPos` option.
	 */
	[[nodiscard]] const std::vector<double> &get_scale() const { return scale; }

	std::vector<double> &get_mutable_scale() { return scale; }

	void set_scale(const std::vector<double> &value) { this->scale = value; }

	/**
	 * An array containing the `[x,y]` pixel coordinates of the top-left corner of the
	 * **cropped** background image, depending on `bgPos` option.
	 */
	[[nodiscard]] const std::vector<int64_t> &get_top_left_px() const { return top_left_px; }

	std::vector<int64_t> &get_mutable_top_left_px() { return top_left_px; }

	void set_top_left_px(const std::vector<int64_t> &value) { this->top_left_px = value; }
};

enum class BgPos : int
{
	CONTAIN,
	COVER,
	COVER_DIRTY,
	REPEAT,
	UNSCALED
};

/**
 * Nearby level info
 */
class WVK_API NeighbourLevel
{
  public:
	NeighbourLevel()          = default;
	virtual ~NeighbourLevel() = default;

  private:
	string            dir;
	string            level_iid;
	optional<int64_t> level_uid;

  public:
	/**
	 * A lowercase string tipping on the level location (`n`orth, `s`outh, `w`est,
	 * `e`ast).<br/>  Since 1.4.0, this value can also be `<` (neighbour depth is lower), `>`
	 * (neighbour depth is greater) or `o` (levels overlap and share the same world
	 * depth).<br/>  Since 1.5.3, this value can also be `nw`,`ne`,`sw` or `se` for levels only
	 * touching corners.
	 */
	[[nodiscard]] const string &get_dir() const { return dir; }

	string &get_mutable_dir() { return dir; }

	void set_dir(const string &value) { this->dir = value; }

	/**
	 * Neighbour Instance Identifier
	 */
	[[nodiscard]] const string &get_level_iid() const { return level_iid; }

	string &get_mutable_level_iid() { return level_iid; }

	void set_level_iid(const string &value) { this->level_iid = value; }
};

/**
 * This section contains all the level data. It can be found in 2 distinct forms, depending on Project current settings:
 * - If Separate level files is disabled (default): full level data is embedded inside the main Project JSON file,
 * - If Separate level files is enabled:
 * level data is stored in *separate* standalone `.ldtkl` files
 * (one per level). In this case, the main Project JSON file will still contain most level data,
 * except heavy sections, like the `layerInstances` array (which will be null). The
 * `externalRelPath` string points to the `ldtkl` file.  A `ldtkl` file is just a JSON file
 * containing exactly what is described below.
 */
class WVK_API LdtkLevel
{
  public:
	LdtkLevel()          = default;
	virtual ~LdtkLevel() = default;

  private:
	string                            bg_color;
	optional<LevelBackgroundPosition> bg_pos;
	std::vector<NeighbourLevel>       neighbours;
	string                            smart_color;
	optional<string>                  level_bg_color;
	double                            bg_pivot_x{};
	double                            bg_pivot_y{};
	optional<BgPos>                   level_bg_pos;
	optional<string>                  bg_rel_path;
	optional<string>                  external_rel_path;
	std::vector<FieldInstance>        field_instances;
	string                            identifier;
	string                            iid;
	optional<std::vector<LdtkLayer>>  layer_instances;
	int64_t                           px_hei{};
	int64_t                           px_wid{};
	int64_t                           uid{};
	bool                              use_auto_identifier{};
	int64_t                           world_depth{};
	int64_t                           world_x{};
	int64_t                           world_y{};

  public:
	/**
	 * Background color of the level (same as `bgColor`, except the default value is
	 * automatically used here if its value is `null`)
	 */
	[[nodiscard]] const string &get_bg_color() const { return bg_color; }

	string &get_mutable_bg_color() { return bg_color; }

	void set_bg_color(const string &value) { this->bg_color = value; }

	/**
	 * Position informations of the background image, if there is one.
	 */
	[[nodiscard]] optional<LevelBackgroundPosition> get_bg_pos() const { return bg_pos; }

	void set_bg_pos(optional<LevelBackgroundPosition> value) { this->bg_pos = std::move(value); }

	/**
	 * An array listing all other levels touching this one on the world map. Since 1.4.0, this
	 * includes levels that overlap in the same world layer, or in nearby world layers.<br/>
	 * Only relevant for world layouts where level spatial positioning is manual (ie. GridVania,
	 * Free). For Horizontal and Vertical layouts, this array is always empty.
	 */
	[[nodiscard]] const std::vector<NeighbourLevel> &get_neighbours() const { return neighbours; }

	std::vector<NeighbourLevel> &get_mutable_neighbours() { return neighbours; }

	void set_neighbours(const std::vector<NeighbourLevel> &value) { this->neighbours = value; }

	/**
	 * The "guessed" color for this level in the editor, decided using either the background
	 * color or an existing custom field.
	 */
	[[nodiscard]] const string &get_smart_color() const { return smart_color; }

	string &get_mutable_smart_color() { return smart_color; }

	void set_smart_color(const string &value) { this->smart_color = value; }

	/**
	 * Background color of the level. If `null`, the project `defaultLevelBgColor` should be
	 * used.
	 */
	[[nodiscard]] optional<string> get_level_bg_color() const { return level_bg_color; }

	void set_level_bg_color(optional<string> value) { this->level_bg_color = std::move(value); }

	/**
	 * Background image X pivot (0-1)
	 */
	[[nodiscard]] const double &get_bg_pivot_x() const { return bg_pivot_x; }

	double &get_mutable_bg_pivot_x() { return bg_pivot_x; }

	void set_bg_pivot_x(const double &value) { this->bg_pivot_x = value; }

	/**
	 * Background image Y pivot (0-1)
	 */
	[[nodiscard]] const double &get_bg_pivot_y() const { return bg_pivot_y; }

	double &get_mutable_bg_pivot_y() { return bg_pivot_y; }

	void set_bg_pivot_y(const double &value) { this->bg_pivot_y = value; }

	/**
	 * An enum defining the way the background image (if any) is positioned on the level. See
	 * `__bgPos` for resulting position info. Possible values: &lt;`null`&gt;, `Unscaled`,
	 * `Contain`, `Cover`, `CoverDirty`, `Repeat`
	 */
	[[nodiscard]] optional<BgPos> get_level_bg_pos() const { return level_bg_pos; }

	void set_level_bg_pos(optional<BgPos> value) { this->level_bg_pos = value; }

	/**
	 * The *optional* relative path to the level background image.
	 */
	[[nodiscard]] optional<string> get_bg_rel_path() const { return bg_rel_path; }

	void set_bg_rel_path(optional<string> value) { this->bg_rel_path = std::move(value); }

	/**
	 * This value is not null if the project option "*Save levels separately*" is enabled. In
	 * this case, this **relative** path points to the level Json file.
	 */
	[[nodiscard]] optional<string> get_external_rel_path() const { return external_rel_path; }

	void set_external_rel_path(optional<string> value) { this->external_rel_path = std::move(value); }

	/**
	 * An array containing this level custom field values.
	 */
	[[nodiscard]] const std::vector<FieldInstance> &get_field_instances() const { return field_instances; }

	std::vector<FieldInstance> &get_mutable_field_instances() { return field_instances; }

	void set_field_instances(const std::vector<FieldInstance> &value) { this->field_instances = value; }

	/**
	 * User defined unique identifier
	 */
	[[nodiscard]] const string &get_identifier() const { return identifier; }

	string &get_mutable_identifier() { return identifier; }

	void set_identifier(const string &value) { this->identifier = value; }

	/**
	 * Unique instance identifier
	 */
	[[nodiscard]] const string &get_iid() const { return iid; }

	string &get_mutable_iid() { return iid; }

	void set_iid(const string &value) { this->iid = value; }

	/**
	 * An array containing all Layer instances. **IMPORTANT**: if the project option "*Save
	 * levels separately*" is enabled, this field will be `null`.<br/>  This array is **sorted
	 * in display order**: the 1st layer is the top-most and the last is behind.
	 */
	[[nodiscard]] optional<std::vector<LdtkLayer>> get_layer_instances() const { return layer_instances; }

	void set_layer_instances(optional<std::vector<LdtkLayer>> value) { this->layer_instances = std::move(value); }

	/**
	 * Height of the level in pixels
	 */
	[[nodiscard]] const int64_t &get_px_hei() const { return px_hei; }

	int64_t &get_mutable_px_hei() { return px_hei; }

	void set_px_hei(const int64_t &value) { this->px_hei = value; }

	/**
	 * Width of the level in pixels
	 */
	[[nodiscard]] const int64_t &get_px_wid() const { return px_wid; }

	int64_t &get_mutable_px_wid() { return px_wid; }

	void set_px_wid(const int64_t &value) { this->px_wid = value; }

	/**
	 * Unique Int identifier
	 */
	[[nodiscard]] const int64_t &get_uid() const { return uid; }

	int64_t &get_mutable_uid() { return uid; }

	void set_uid(const int64_t &value) { this->uid = value; }

	/**
	 * If TRUE, the level identifier will always automatically use the naming pattern as defined
	 * in `Project.levelNamePattern`. Becomes FALSE if the identifier is manually modified by
	 * user.
	 */
	[[nodiscard]] const bool &get_use_auto_identifier() const { return use_auto_identifier; }

	bool &get_mutable_use_auto_identifier() { return use_auto_identifier; }

	void set_use_auto_identifier(const bool &value) { this->use_auto_identifier = value; }

	/**
	 * Index that represents the "depth" of the level in the world. Default is 0, greater means
	 * "above", lower means "below".<br/>  This value is mostly used for display only and is
	 * intended to make stacking of levels easier to manage.
	 */
	[[nodiscard]] const int64_t &get_world_depth() const { return world_depth; }

	int64_t &get_mutable_world_depth() { return world_depth; }

	void set_world_depth(const int64_t &value) { this->world_depth = value; }

	/**
	 * World X coordinate in pixels.<br/>  Only relevant for world layouts where level spatial
	 * positioning is manual (ie. GridVania, Free). For Horizontal and Vertical layouts, the
	 * value is always -1 here.
	 */
	[[nodiscard]] const int64_t &get_world_x() const { return world_x; }

	int64_t &get_mutable_world_x() { return world_x; }

	void set_world_x(const int64_t &value) { this->world_x = value; }

	/**
	 * World Y coordinate in pixels.<br/>  Only relevant for world layouts where level spatial
	 * positioning is manual (ie. GridVania, Free). For Horizontal and Vertical layouts, the
	 * value is always -1 here.
	 */
	[[nodiscard]] const int64_t &get_world_y() const { return world_y; }

	int64_t &get_mutable_world_y() { return world_y; }

	void set_world_y(const int64_t &value) { this->world_y = value; }
};

class WVK_API LdtkTocInstanceData
{
  public:
	LdtkTocInstanceData()          = default;
	virtual ~LdtkTocInstanceData() = default;

  private:
	json                        fields;
	int64_t                     hei_px{};
	ReferenceToAnEntityInstance iids{};
	int64_t                     wid_px{};
	int64_t                     world_x{};
	int64_t                     world_y{};

  public:
	/**
	 * An object containing the values of all entity fields with the `exportToToc` option
	 * enabled. This object typing depends on actual field value types.
	 */
	[[nodiscard]] const json &get_fields() const { return fields; }

	json &get_mutable_fields() { return fields; }

	void set_fields(const json &value) { this->fields = value; }

	[[nodiscard]] const int64_t &get_hei_px() const { return hei_px; }

	int64_t &get_mutable_hei_px() { return hei_px; }

	void set_hei_px(const int64_t &value) { this->hei_px = value; }

	/**
	 * IID information of this instance
	 */
	[[nodiscard]] const ReferenceToAnEntityInstance &get_iids() const { return iids; }

	ReferenceToAnEntityInstance &get_mutable_iids() { return iids; }

	void set_iids(const ReferenceToAnEntityInstance &value) { this->iids = value; }

	[[nodiscard]] const int64_t &get_wid_px() const { return wid_px; }

	int64_t &get_mutable_wid_px() { return wid_px; }

	void set_wid_px(const int64_t &value) { this->wid_px = value; }

	[[nodiscard]] const int64_t &get_world_x() const { return world_x; }

	int64_t &get_mutable_world_x() { return world_x; }

	void set_world_x(const int64_t &value) { this->world_x = value; }

	[[nodiscard]] const int64_t &get_world_y() const { return world_y; }

	int64_t &get_mutable_world_y() { return world_y; }

	void set_world_y(const int64_t &value) { this->world_y = value; }
};

class WVK_API LdtkTableOfContentEntry
{
  public:
	LdtkTableOfContentEntry()          = default;
	virtual ~LdtkTableOfContentEntry() = default;

  private:
	string                                             identifier;
	optional<std::vector<ReferenceToAnEntityInstance>> instances;
	std::vector<LdtkTocInstanceData>                   instances_data;

  public:
	[[nodiscard]] const string &get_identifier() const { return identifier; }

	string &get_mutable_identifier() { return identifier; }

	void set_identifier(const string &value) { this->identifier = value; }

	/**
	 * **WARNING**: this deprecated value will be *removed* completely on version 1.7.0+
	 * Replaced by: `instancesData`
	 */
	[[nodiscard]] optional<std::vector<ReferenceToAnEntityInstance>> get_instances() const { return instances; }

	void set_instances(optional<std::vector<ReferenceToAnEntityInstance>> value) { this->instances = std::move(value); }

	[[nodiscard]] const std::vector<LdtkTocInstanceData> &get_instances_data() const { return instances_data; }

	std::vector<LdtkTocInstanceData> &get_mutable_instances_data() { return instances_data; }

	void set_instances_data(const std::vector<LdtkTocInstanceData> &value) { this->instances_data = value; }
};

enum class WorldLayout : int
{
	FREE,
	GRID_VANIA,
	LINEAR_HORIZONTAL,
	LINEAR_VERTICAL
};

/**
 * **IMPORTANT**: this type is available as a preview. You can rely on it to update your
 * importers, for when it will be officially available.  A World contains multiple levels,
 * and it has its own layout settings.
 */
class WVK_API LdtkWorld
{
  public:
	LdtkWorld()          = default;
	virtual ~LdtkWorld() = default;

  private:
	int64_t                default_level_height{};
	int64_t                default_level_width{};
	string                 identifier;
	string                 iid;
	std::vector<LdtkLevel> levels;
	int64_t                world_grid_height{};
	int64_t                world_grid_width{};
	optional<WorldLayout>  world_layout;

  public:
	/**
	 * Default new level height
	 */
	[[nodiscard]] const int64_t &get_default_level_height() const { return default_level_height; }

	int64_t &get_mutable_default_level_height() { return default_level_height; }

	void set_default_level_height(const int64_t &value) { this->default_level_height = value; }

	/**
	 * Default new level width
	 */
	[[nodiscard]] const int64_t &get_default_level_width() const { return default_level_width; }

	int64_t &get_mutable_default_level_width() { return default_level_width; }

	void set_default_level_width(const int64_t &value) { this->default_level_width = value; }

	/**
	 * User defined unique identifier
	 */
	[[nodiscard]] const string &get_identifier() const { return identifier; }

	string &get_mutable_identifier() { return identifier; }

	void set_identifier(const string &value) { this->identifier = value; }

	/**
	 * Unique instance identifer
	 */
	[[nodiscard]] const string &get_iid() const { return iid; }

	string &get_mutable_iid() { return iid; }

	void set_iid(const string &value) { this->iid = value; }

	/**
	 * All levels from this world. The order of this array is only relevant in
	 * `LinearHorizontal` and `linearVertical` world layouts (see `worldLayout` value).
	 * Otherwise, you should refer to the `worldX`,`worldY` coordinates of each Level.
	 */
	[[nodiscard]] const std::vector<LdtkLevel> &get_levels() const { return levels; }

	std::vector<LdtkLevel> &get_mutable_levels() { return levels; }

	void set_levels(const std::vector<LdtkLevel> &value) { this->levels = value; }

	/**
	 * Height of the world grid in pixels.
	 */
	[[nodiscard]] const int64_t &get_world_grid_height() const { return world_grid_height; }

	int64_t &get_mutable_world_grid_height() { return world_grid_height; }

	void set_world_grid_height(const int64_t &value) { this->world_grid_height = value; }

	/**
	 * Width of the world grid in pixels.
	 */
	[[nodiscard]] const int64_t &get_world_grid_width() const { return world_grid_width; }

	int64_t &get_mutable_world_grid_width() { return world_grid_width; }

	void set_world_grid_width(const int64_t &value) { this->world_grid_width = value; }

	/**
	 * An enum that describes how levels are organized in this project (ie. linearly or in a 2D
	 * space). Possible values: `Free`, `GridVania`, `LinearHorizontal`, `LinearVertical`, `null`
	 */
	[[nodiscard]] optional<WorldLayout> get_world_layout() const { return world_layout; }

	void set_world_layout(optional<WorldLayout> value) { this->world_layout = value; }
};

class WVK_API ForcedRefs
{
  public:
	ForcedRefs()          = default;
	virtual ~ForcedRefs() = default;

  private:
	optional<AutoLayerRuleGroup>          auto_layer_rule_group;
	optional<AutoLayerRuleDefinition>     auto_rule_def;
	optional<LdtkCustomCommand>           custom_command;
	optional<LdtkDefs>                    definitions;
	optional<EntityDefinition>            entity_def;
	optional<EntityInstance>              entity_instance;
	optional<ReferenceToAnEntityInstance> entity_reference_infos;
	optional<EnumDefinition>              enum_def;
	optional<EnumValueDefinition>         enum_def_values;
	optional<EnumTagValue>                enum_tag_value;
	optional<FieldDefinition>             field_def;
	optional<FieldInstance>               field_instance;
	optional<GridPoint>                   grid_point;
	optional<IntGridValueDefinition>      int_grid_value_def;
	optional<IntGridValueGroupDefinition> int_grid_value_group_def;
	optional<IntGridValueInstance>        int_grid_value_instance;
	optional<LayerDefinition>             layer_def;
	optional<LdtkLayer>                   layer_instance;
	optional<LdtkLevel>                   level;
	optional<LevelBackgroundPosition>     level_bg_pos_infos;
	optional<NeighbourLevel>              neighbour_level;
	optional<LdtkTableOfContentEntry>     table_of_content_entry;
	optional<TileInstance>                tile;
	optional<TileCustomMetadata>          tile_custom_metadata;
	optional<TilesetDefinition>           tileset_def;
	optional<TilesetRectangle>            tileset_rect;
	optional<LdtkTocInstanceData>         toc_instance_data;
	optional<LdtkWorld>                   world;

  public:
	[[nodiscard]] optional<AutoLayerRuleGroup> get_auto_layer_rule_group() const { return auto_layer_rule_group; }

	void set_auto_layer_rule_group(optional<AutoLayerRuleGroup> value) { this->auto_layer_rule_group = std::move(value); }

	[[nodiscard]] optional<AutoLayerRuleDefinition> get_auto_rule_def() const { return auto_rule_def; }

	void set_auto_rule_def(optional<AutoLayerRuleDefinition> value) { this->auto_rule_def = std::move(value); }

	[[nodiscard]] optional<LdtkCustomCommand> get_custom_command() const { return custom_command; }

	void set_custom_command(optional<LdtkCustomCommand> value) { this->custom_command = std::move(value); }

	[[nodiscard]] optional<LdtkDefs> get_definitions() const { return definitions; }

	void set_definitions(optional<LdtkDefs> value) { this->definitions = std::move(value); }

	[[nodiscard]] optional<EntityDefinition> get_entity_def() const { return entity_def; }

	void set_entity_def(optional<EntityDefinition> value) { this->entity_def = std::move(value); }

	[[nodiscard]] optional<EntityInstance> get_entity_instance() const { return entity_instance; }

	void set_entity_instance(optional<EntityInstance> value) { this->entity_instance = std::move(value); }

	[[nodiscard]] optional<ReferenceToAnEntityInstance> get_entity_reference_infos() const { return entity_reference_infos; }

	void set_entity_reference_infos(optional<ReferenceToAnEntityInstance> value) { this->entity_reference_infos = std::move(value); }

	[[nodiscard]] optional<EnumDefinition> get_enum_def() const { return enum_def; }

	void set_enum_def(optional<EnumDefinition> value) { this->enum_def = std::move(value); }

	[[nodiscard]] optional<EnumValueDefinition> get_enum_def_values() const { return enum_def_values; }

	void set_enum_def_values(optional<EnumValueDefinition> value) { this->enum_def_values = std::move(value); }

	[[nodiscard]] optional<EnumTagValue> get_enum_tag_value() const { return enum_tag_value; }

	void set_enum_tag_value(optional<EnumTagValue> value) { this->enum_tag_value = std::move(value); }

	[[nodiscard]] optional<FieldDefinition> get_field_def() const { return field_def; }

	void set_field_def(optional<FieldDefinition> value) { this->field_def = std::move(value); }

	[[nodiscard]] optional<FieldInstance> get_field_instance() const { return field_instance; }

	void set_field_instance(optional<FieldInstance> value) { this->field_instance = std::move(value); }

	[[nodiscard]] optional<GridPoint> get_grid_point() const { return grid_point; }

	void set_grid_point(optional<GridPoint> value) { this->grid_point = std::move(value); }

	[[nodiscard]] optional<IntGridValueDefinition> get_int_grid_value_def() const { return int_grid_value_def; }

	void set_int_grid_value_def(optional<IntGridValueDefinition> value) { this->int_grid_value_def = std::move(value); }

	[[nodiscard]] optional<IntGridValueGroupDefinition> get_int_grid_value_group_def() const { return int_grid_value_group_def; }

	void set_int_grid_value_group_def(optional<IntGridValueGroupDefinition> value) { this->int_grid_value_group_def = std::move(value); }

	[[nodiscard]] optional<IntGridValueInstance> get_int_grid_value_instance() const { return int_grid_value_instance; }

	void set_int_grid_value_instance(optional<IntGridValueInstance> value) { this->int_grid_value_instance = std::move(value); }

	[[nodiscard]] optional<LayerDefinition> get_layer_def() const { return layer_def; }

	void set_layer_def(optional<LayerDefinition> value) { this->layer_def = std::move(value); }

	[[nodiscard]] optional<LdtkLayer> get_layer_instance() const { return layer_instance; }

	void set_layer_instance(optional<LdtkLayer> value) { this->layer_instance = std::move(value); }

	[[nodiscard]] optional<LdtkLevel> get_level() const { return level; }

	void set_level(optional<LdtkLevel> value) { this->level = std::move(value); }

	[[nodiscard]] optional<LevelBackgroundPosition> get_level_bg_pos_infos() const { return level_bg_pos_infos; }

	void set_level_bg_pos_infos(optional<LevelBackgroundPosition> value) { this->level_bg_pos_infos = std::move(value); }

	[[nodiscard]] optional<NeighbourLevel> get_neighbour_level() const { return neighbour_level; }

	void set_neighbour_level(optional<NeighbourLevel> value) { this->neighbour_level = std::move(value); }

	[[nodiscard]] optional<LdtkTableOfContentEntry> get_table_of_content_entry() const { return table_of_content_entry; }

	void set_table_of_content_entry(optional<LdtkTableOfContentEntry> value) { this->table_of_content_entry = std::move(value); }

	[[nodiscard]] optional<TileInstance> get_tile() const { return tile; }

	void set_tile(optional<TileInstance> value) { this->tile = std::move(value); }

	[[nodiscard]] optional<TileCustomMetadata> get_tile_custom_metadata() const { return tile_custom_metadata; }

	void set_tile_custom_metadata(optional<TileCustomMetadata> value) { this->tile_custom_metadata = std::move(value); }

	[[nodiscard]] optional<TilesetDefinition> get_tileset_def() const { return tileset_def; }

	void set_tileset_def(optional<TilesetDefinition> value) { this->tileset_def = std::move(value); }

	[[nodiscard]] optional<TilesetRectangle> get_tileset_rect() const { return tileset_rect; }

	void set_tileset_rect(optional<TilesetRectangle> value) { this->tileset_rect = std::move(value); }

	[[nodiscard]] optional<LdtkTocInstanceData> get_toc_instance_data() const { return toc_instance_data; }

	void set_toc_instance_data(optional<LdtkTocInstanceData> value) { this->toc_instance_data = std::move(value); }

	[[nodiscard]] optional<LdtkWorld> get_world() const { return world; }

	void set_world(optional<LdtkWorld> value) { this->world = std::move(value); }
};

/**
 * Naming convention for Identifiers (first-letter uppercase, full uppercase etc.) Possible
 * values: `Capitalize`, `Uppercase`, `Lowercase`, `Free`
 */
enum class IdentifierStyle : int
{
	CAPITALIZE,
	FREE,
	LOWERCASE,
	UPPERCASE
};

/**
 * "Image export" option when saving project. Possible values: `None`, `OneImagePerLayer`,
 * `OneImagePerLevel`, `LayersAndLevels`
 */
enum class ImageExportMode : int
{
	LAYERS_AND_LEVELS,
	NONE,
	ONE_IMAGE_PER_LAYER,
	ONE_IMAGE_PER_LEVEL
};

/**
 * This file is a JSON schema of files created by LDtk level editor (https://ldtk.io).
 *
 * This is the root of any Project JSON file. It contains:  - the project settings, - an
 * array of levels, - a group of definitions (that can probably be safely ignored for most
 * users).
 */
class WVK_API LdtkJson
{
  public:
	LdtkJson()          = default;
	virtual ~LdtkJson() = default;

  private:
	optional<ForcedRefs>                 forced_refs;
	double                               app_build_id{};
	int64_t                              backup_limit{};
	bool                                 backup_on_save{};
	optional<string>                     backup_rel_path;
	string                               bg_color;
	std::vector<LdtkCustomCommand>       custom_commands;
	int64_t                              default_entity_height{};
	int64_t                              default_entity_width{};
	int64_t                              default_grid_size{};
	string                               default_level_bg_color;
	optional<int64_t>                    default_level_height;
	optional<int64_t>                    default_level_width;
	double                               default_pivot_x{};
	double                               default_pivot_y{};
	LdtkDefs                             defs{};
	string                               dummy_world_iid;
	bool                                 export_level_bg{};
	optional<bool>                       export_png;
	bool                                 export_tiled{};
	bool                                 external_levels{};
	std::vector<Flag>                    flags;
	IdentifierStyle                      identifier_style{};
	string                               iid;
	ImageExportMode                      image_export_mode{};
	string                               json_version;
	string                               level_name_pattern;
	std::vector<LdtkLevel>               levels;
	bool                                 minify_json{};
	int64_t                              next_uid{};
	optional<string>                     png_file_pattern;
	bool                                 simplified_export{};
	std::vector<LdtkTableOfContentEntry> toc;
	optional<string>                     tutorial_desc;
	optional<int64_t>                    world_grid_height;
	optional<int64_t>                    world_grid_width;
	optional<WorldLayout>                world_layout;
	std::vector<LdtkWorld>               worlds;

  public:
	/**
	 * This object is not actually used by LDtk. It ONLY exists to force explicit references to
	 * all types, to make sure QuickType finds them and integrate all of them. Otherwise,
	 * Quicktype will drop types that are not explicitely used.
	 */
	[[nodiscard]] optional<ForcedRefs> get_forced_refs() const { return forced_refs; }

	void set_forced_refs(optional<ForcedRefs> value) { this->forced_refs = std::move(value); }

	/**
	 * LDtk application build identifier.<br/>  This is only used to identify the LDtk version
	 * that generated this particular project file, which can be useful for specific bug fixing.
	 * Note that the build identifier is just the date of the release, so it's not unique to
	 * each user (one single global ID per LDtk public release), and as a result, completely
	 * anonymous.
	 */
	[[nodiscard]] const double &get_app_build_id() const { return app_build_id; }

	double &get_mutable_app_build_id() { return app_build_id; }

	void set_app_build_id(const double &value) { this->app_build_id = value; }

	/**
	 * Number of backup files to keep, if the `backupOnSave` is TRUE
	 */
	[[nodiscard]] const int64_t &get_backup_limit() const { return backup_limit; }

	int64_t &get_mutable_backup_limit() { return backup_limit; }

	void set_backup_limit(const int64_t &value) { this->backup_limit = value; }

	/**
	 * If TRUE, an extra copy of the project will be created in a sub folder, when saving.
	 */
	[[nodiscard]] const bool &get_backup_on_save() const { return backup_on_save; }

	bool &get_mutable_backup_on_save() { return backup_on_save; }

	void set_backup_on_save(const bool &value) { this->backup_on_save = value; }

	/**
	 * Target relative path to store backup files
	 */
	[[nodiscard]] optional<string> get_backup_rel_path() const { return backup_rel_path; }

	void set_backup_rel_path(optional<string> value) { this->backup_rel_path = std::move(value); }

	/**
	 * Project background color
	 */
	[[nodiscard]] const string &get_bg_color() const { return bg_color; }

	string &get_mutable_bg_color() { return bg_color; }

	void set_bg_color(const string &value) { this->bg_color = value; }

	/**
	 * An array of command lines that can be ran manually by the user
	 */
	[[nodiscard]] const std::vector<LdtkCustomCommand> &get_custom_commands() const { return custom_commands; }

	std::vector<LdtkCustomCommand> &get_mutable_custom_commands() { return custom_commands; }

	void set_custom_commands(const std::vector<LdtkCustomCommand> &value) { this->custom_commands = value; }

	/**
	 * Default height for new entities
	 */
	[[nodiscard]] const int64_t &get_default_entity_height() const { return default_entity_height; }

	int64_t &get_mutable_default_entity_height() { return default_entity_height; }

	void set_default_entity_height(const int64_t &value) { this->default_entity_height = value; }

	/**
	 * Default width for new entities
	 */
	[[nodiscard]] const int64_t &get_default_entity_width() const { return default_entity_width; }

	int64_t &get_mutable_default_entity_width() { return default_entity_width; }

	void set_default_entity_width(const int64_t &value) { this->default_entity_width = value; }

	/**
	 * Default grid size for new layers
	 */
	[[nodiscard]] const int64_t &get_default_grid_size() const { return default_grid_size; }

	int64_t &get_mutable_default_grid_size() { return default_grid_size; }

	void set_default_grid_size(const int64_t &value) { this->default_grid_size = value; }

	/**
	 * Default background color of levels
	 */
	[[nodiscard]] const string &get_default_level_bg_color() const { return default_level_bg_color; }

	string &get_mutable_default_level_bg_color() { return default_level_bg_color; }

	void set_default_level_bg_color(const string &value) { this->default_level_bg_color = value; }

	/**
	 * **WARNING**: this field will move to the `worlds` array after the "multi-worlds" update.
	 * It will then be `null`. You can enable the Multi-worlds advanced project option to enable
	 * the change immediately.<br/><br/>  Default new level height
	 */
	[[nodiscard]] optional<int64_t> get_default_level_height() const { return default_level_height; }

	void set_default_level_height(optional<int64_t> value) { this->default_level_height = value; }

	/**
	 * **WARNING**: this field will move to the `worlds` array after the "multi-worlds" update.
	 * It will then be `null`. You can enable the Multi-worlds advanced project option to enable
	 * the change immediately.<br/><br/>  Default new level width
	 */
	[[nodiscard]] optional<int64_t> get_default_level_width() const { return default_level_width; }

	void set_default_level_width(optional<int64_t> value) { this->default_level_width = value; }

	/**
	 * Default X pivot (0 to 1) for new entities
	 */
	[[nodiscard]] const double &get_default_pivot_x() const { return default_pivot_x; }

	double &get_mutable_default_pivot_x() { return default_pivot_x; }

	void set_default_pivot_x(const double &value) { this->default_pivot_x = value; }

	/**
	 * Default Y pivot (0 to 1) for new entities
	 */
	[[nodiscard]] const double &get_default_pivot_y() const { return default_pivot_y; }

	double &get_mutable_default_pivot_y() { return default_pivot_y; }

	void set_default_pivot_y(const double &value) { this->default_pivot_y = value; }

	/**
	 * A structure containing all the definitions of this project
	 */
	[[nodiscard]] const LdtkDefs &get_defs() const { return defs; }

	LdtkDefs &get_mutable_defs() { return defs; }

	void set_defs(const LdtkDefs &value) { this->defs = value; }

	/**
	 * If the project isn't in MultiWorlds mode, this is the IID of the internal "dummy" World.
	 */
	[[nodiscard]] const string &get_dummy_world_iid() const { return dummy_world_iid; }

	string &get_mutable_dummy_world_iid() { return dummy_world_iid; }

	void set_dummy_world_iid(const string &value) { this->dummy_world_iid = value; }

	/**
	 * If TRUE, the exported PNGs will include the level background (color or image).
	 */
	[[nodiscard]] const bool &get_export_level_bg() const { return export_level_bg; }

	bool &get_mutable_export_level_bg() { return export_level_bg; }

	void set_export_level_bg(const bool &value) { this->export_level_bg = value; }

	/**
	 * **WARNING**: this deprecated value is no longer exported since version 0.9.3  Replaced
	 * by: `imageExportMode`
	 */
	[[nodiscard]] optional<bool> get_export_png() const { return export_png; }

	void set_export_png(optional<bool> value) { this->export_png = value; }

	/**
	 * If TRUE, a Tiled compatible file will also be generated along with the LDtk JSON file
	 * (default is FALSE)
	 */
	[[nodiscard]] const bool &get_export_tiled() const { return export_tiled; }

	bool &get_mutable_export_tiled() { return export_tiled; }

	void set_export_tiled(const bool &value) { this->export_tiled = value; }

	/**
	 * If TRUE, one file will be saved for the project (incl. all its definitions) and one file
	 * in a sub-folder for each level.
	 */
	[[nodiscard]] const bool &get_external_levels() const { return external_levels; }

	bool &get_mutable_external_levels() { return external_levels; }

	void set_external_levels(const bool &value) { this->external_levels = value; }

	/**
	 * An array containing various advanced flags (ie. options or other states). Possible
	 * values: `DiscardPreCsvIntGrid`, `ExportOldTableOfContentData`,
	 * `ExportPreCsvIntGridFormat`, `IgnoreBackupSuggest`, `PrependIndexToLevelFileNames`,
	 * `MultiWorlds`, `UseMultilinesType`
	 */
	[[nodiscard]] const std::vector<Flag> &get_flags() const { return flags; }

	std::vector<Flag> &get_mutable_flags() { return flags; }

	void set_flags(const std::vector<Flag> &value) { this->flags = value; }

	/**
	 * Naming convention for Identifiers (first-letter uppercase, full uppercase etc.) Possible
	 * values: `Capitalize`, `Uppercase`, `Lowercase`, `Free`
	 */
	[[nodiscard]] const IdentifierStyle &get_identifier_style() const { return identifier_style; }

	IdentifierStyle &get_mutable_identifier_style() { return identifier_style; }

	void set_identifier_style(const IdentifierStyle &value) { this->identifier_style = value; }

	/**
	 * Unique project identifier
	 */
	[[nodiscard]] const string &get_iid() const { return iid; }

	string &get_mutable_iid() { return iid; }

	void set_iid(const string &value) { this->iid = value; }

	/**
	 * "Image export" option when saving project. Possible values: `None`, `OneImagePerLayer`,
	 * `OneImagePerLevel`, `LayersAndLevels`
	 */
	[[nodiscard]] const ImageExportMode &get_image_export_mode() const { return image_export_mode; }

	ImageExportMode &get_mutable_image_export_mode() { return image_export_mode; }

	void set_image_export_mode(const ImageExportMode &value) { this->image_export_mode = value; }

	/**
	 * File format version
	 */
	[[nodiscard]] const string &get_json_version() const { return json_version; }

	string &get_mutable_json_version() { return json_version; }

	void set_json_version(const string &value) { this->json_version = value; }

	/**
	 * The default naming convention for level identifiers.
	 */
	[[nodiscard]] const string &get_level_name_pattern() const { return level_name_pattern; }

	string &get_mutable_level_name_pattern() { return level_name_pattern; }

	void set_level_name_pattern(const string &value) { this->level_name_pattern = value; }

	/**
	 * All levels. The order of this array is only relevant in `LinearHorizontal` and
	 * `linearVertical` world layouts (see `worldLayout` value).<br/>  Otherwise, you should
	 * refer to the `worldX`,`worldY` coordinates of each Level.
	 */
	[[nodiscard]] const std::vector<LdtkLevel> &get_levels() const { return levels; }

	std::vector<LdtkLevel> &get_mutable_levels() { return levels; }

	void set_levels(const std::vector<LdtkLevel> &value) { this->levels = value; }

	/**
	 * If TRUE, the Json is partially minified (no indentation, nor line breaks, default is
	 * FALSE)
	 */
	[[nodiscard]] const bool &get_minify_json() const { return minify_json; }

	bool &get_mutable_minify_json() { return minify_json; }

	void set_minify_json(const bool &value) { this->minify_json = value; }

	/**
	 * Next Unique integer ID available
	 */
	[[nodiscard]] const int64_t &get_next_uid() const { return next_uid; }

	int64_t &get_mutable_next_uid() { return next_uid; }

	void set_next_uid(const int64_t &value) { this->next_uid = value; }

	/**
	 * File naming pattern for exported PNGs
	 */
	[[nodiscard]] optional<string> get_png_file_pattern() const { return png_file_pattern; }

	void set_png_file_pattern(optional<string> value) { this->png_file_pattern = std::move(value); }

	/**
	 * If TRUE, a very simplified will be generated on saving, for quicker & easier engine
	 * integration.
	 */
	[[nodiscard]] const bool &get_simplified_export() const { return simplified_export; }

	bool &get_mutable_simplified_export() { return simplified_export; }

	void set_simplified_export(const bool &value) { this->simplified_export = value; }

	/**
	 * All instances of entities that have their `exportToToc` flag enabled are listed in this
	 * array.
	 */
	[[nodiscard]] const std::vector<LdtkTableOfContentEntry> &get_toc() const { return toc; }

	std::vector<LdtkTableOfContentEntry> &get_mutable_toc() { return toc; }

	void set_toc(const std::vector<LdtkTableOfContentEntry> &value) { this->toc = value; }

	/**
	 * This optional description is used by LDtk Samples to show up some informations and
	 * instructions.
	 */
	[[nodiscard]] optional<string> get_tutorial_desc() const { return tutorial_desc; }
	void                           set_tutorial_desc(optional<string> value) { this->tutorial_desc = std::move(value); }

	/**
	 * **WARNING**: this field will move to the `worlds` array after the "multi-worlds" update.
	 * It will then be `null`. You can enable the Multi-worlds advanced project option to enable
	 * the change immediately.<br/><br/>  Height of the world grid in pixels.
	 */
	[[nodiscard]] optional<int64_t> get_world_grid_height() const { return world_grid_height; }
	void                            set_world_grid_height(optional<int64_t> value) { this->world_grid_height = value; }

	/**
	 * **WARNING**: this field will move to the `worlds` array after the "multi-worlds" update.
	 * It will then be `null`. You can enable the Multi-worlds advanced project option to enable
	 * the change immediately.<br/><br/>  Width of the world grid in pixels.
	 */
	[[nodiscard]] optional<int64_t> get_world_grid_width() const { return world_grid_width; }

	void set_world_grid_width(optional<int64_t> value) { this->world_grid_width = value; }

	/**
	 * **WARNING**: this field will move to the `worlds` array after the "multi-worlds" update.
	 * It will then be `null`. You can enable the Multi-worlds advanced project option to enable
	 * the change immediately.<br/><br/>  An enum that describes how levels are organized in
	 * this project (ie. linearly or in a 2D space). Possible values: &lt;`null`&gt;, `Free`,
	 * `GridVania`, `LinearHorizontal`, `LinearVertical`
	 */
	[[nodiscard]] optional<WorldLayout> get_world_layout() const { return world_layout; }

	void set_world_layout(optional<WorldLayout> value) { this->world_layout = value; }

	/**
	 * This array will be empty, unless you enable the Multi-Worlds in the project advanced
	 * settings.<br/><br/> - in current version, a LDtk project file can only contain a single
	 * world with multiple levels in it. In this case, levels and world layout related settings
	 * are stored in the root of the JSON.<br/> - with "Multi-worlds" enabled, there will be a
	 * `worlds` array in root, each world containing levels and layout settings. Basically, it's
	 * pretty much only about moving the `levels` array to the `worlds` array, along with world
	 * layout related values (eg. `worldGridWidth` etc).<br/><br/>If you want to start
	 * supporting this future update easily, please refer to this documentation:
	 * https://github.com/deepnight/ldtk/issues/231
	 */
	[[nodiscard]] const std::vector<LdtkWorld> &get_worlds() const { return worlds; }

	std::vector<LdtkWorld> &get_mutable_worlds() { return worlds; }

	void set_worlds(const std::vector<LdtkWorld> &value) { this->worlds = value; }
};

struct Rectangle;

WVK_API void load_ldtk_json(const std::filesystem::path &path, LdtkJson *json);

WVK_API Rectangle tile_id_to_texture_rect(TileId tileId, const glm::ivec2 &tilesetTextureSize);

WVK_API std::pair<glm::vec2, glm::vec2> tile_id_to_uvs(TileId tileId, const glm::ivec2 &tilesetTextureSize);

WVK_API TileIndex world_pos_to_tile_index(const glm::vec2 &worldPos);

WVK_API glm::vec2 tile_index_to_world_pos(const TileIndex &tileIndex);

}        // namespace wvk::core
