#include "LL_GridTile.h"

namespace LL {
	GridPosition GridTile::GetGridPosition() const { return gridPosition; }
	uint32_t GridTile::GetId() const { return id; }
	bool GridTile::GetObstacle() const { return obstacle; }

	void GridTile::SetObstacle(bool obstacle) { this->obstacle = obstacle; }
}