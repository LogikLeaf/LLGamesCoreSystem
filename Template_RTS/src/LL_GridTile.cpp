#include "LL_GridTile.h"

namespace LL::Grid {
	GridPosition Tile::GetGridPosition() const { return gridPosition; }
	uint32_t Tile::GetId() const { return id; }
	bool Tile::GetObstacle() const { return obstacle; }

	void Tile::SetObstacle(bool obstacle) { this->obstacle = obstacle; }
}