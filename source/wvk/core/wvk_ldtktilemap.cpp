#include "wvk_ldtktilemap.hpp"

#include "wvk/common/wvk_pch.hpp"
#include "wvk_core.hpp"
#include "wvk_json.hpp"

namespace wvk::core
{

static constexpr auto tileTextureSize = 16;
static constexpr auto tileWorldSize   = glm::vec2{16.f};

void load_ldtk_json(
    const std::filesystem::path &path,
    LdtkJson                    *ldtkJson)
{
	std::ifstream in(path.string());
	if (in.fail())
	{
		WVK_ERROR("failed to open file {} {}", path.string(), strerror(errno));
		return;
	}

	nlohmann::json json = nlohmann::json::parse(in);
	from_json(json, *ldtkJson);
}

Rectangle tile_id_to_texture_rect(
    TileId           tileId,
    const glm::vec2 &tilesetTextureSize)
{
	const auto numTileX =
	    tilesetTextureSize.x / tileTextureSize;
	const auto tilePixelCoords =
	    glm::ivec2{tileId % WVK_CAST(int, numTileX), tileId / numTileX} * tileTextureSize;

	return {glm::vec2{tilePixelCoords}, glm::vec2{tileTextureSize, tileTextureSize}};
}

std::pair<glm::vec2, glm::vec2> tile_id_to_uvs(
    TileId           tileId,
    const glm::vec2 &tilesetTextureSize)
{
	const Rectangle textureRect = tile_id_to_texture_rect(tileId, tilesetTextureSize);

	const auto uv1 =
	    pixel_coord_to_uv(textureRect.get_top_left(), tilesetTextureSize);

	const auto uv2 =
	    pixel_coord_to_uv(textureRect.get_bottom_right(), tilesetTextureSize);

	return {uv1, uv2};
}

TileIndex world_pos_to_tile_index(
    const glm::vec2 &worldPos)
{
	return {};
}

glm::vec2 tile_index_to_world_pos(
    const TileIndex &tileIndex)
{
	return {};
}

}        // namespace wvk::core