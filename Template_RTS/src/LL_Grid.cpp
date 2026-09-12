#include <cmath>

#include "LL_Grid.h"

namespace LL::Grid {

	float Grid::GridXToWorldX(uint32_t xGrid) const
	{
		// Convert grid X to world X (add the world origin)
		return GRID_WORLD_POSITION.x + static_cast<float>(xGrid) + 0.5f;
	}

	float Grid::GridYToWorldY(uint32_t yGrid) const
	{
		// Convert grid Y to world Y (add the world origin)
		return GRID_WORLD_POSITION.y + static_cast<float>(yGrid) + 0.5f;
	}

	float Grid::WorldXToGridX(float xWorld) const
	{
		return xWorld - GRID_WORLD_POSITION.x - 0.5f;
	}

	float Grid::WorldYToGridY(float yWorld) const
	{
		return yWorld - GRID_WORLD_POSITION.y - 0.5f;
	}

	uint32_t Grid::GetTileIdFromGridCoordinates(uint32_t xLocation, uint32_t yLocation) const
	{
		return yLocation * MAP_SIZE_X + xLocation;
	}

	uint32_t Grid::GetYGridFromId(uint32_t tileId) const
	{
		double doubleValue = std::floor(tileId / MAP_SIZE_X);
		uint32_t result = static_cast<uint32_t>(doubleValue);
		return result;
	}

	uint32_t Grid::GetXGridFromId(uint32_t tileId) const
	{
		uint32_t yGrid = GetYGridFromId(tileId);
		return tileId - (yGrid * MAP_SIZE_X);
	}

	uint32_t Grid::GetMapSizeX() const
	{
		return MAP_SIZE_X;
	}

	uint32_t Grid::GetMapSizeY() const
	{
		return MAP_SIZE_Y;
	}

	Tile* Grid::GetTileAt(uint32_t x, uint32_t y)
	{
		size_t id = GetTileIdFromGridCoordinates(x, y);
		if (id >= tiles.size()) return nullptr;
		return tiles[id].get();
	}

	void Grid::SpawnTiles()
	{
		for (uint32_t i = 0; i < NUMBER_OF_TILES; i++) {
			uint32_t xGrid = GetXGridFromId(i);
			uint32_t yGrid = GetYGridFromId(i);

			// compute world position for the tile and pass it to the Tile constructor
			float wx = GridXToWorldX(xGrid);
			float wy = GridYToWorldY(yGrid);

			tiles.push_back(std::make_unique<Tile>(i, GridPosition{ xGrid, yGrid }, Maths::Position{ wx, wy }));
		}
	}
}