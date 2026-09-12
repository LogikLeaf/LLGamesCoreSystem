/*
* Used by the grid system.
*/
#pragma once

#include <cstdint>

#include "LL_Maths.h"

namespace LL::Grid {

	/** Coordinates of a tile within the grid (not world space). */
	struct GridPosition {
		uint32_t x = 0;
		uint32_t y = 0;
	};

	/** Represents a single cell of the grid. */
	class Tile {
	public:
		// Base constructor
		Tile(const uint32_t& id,
			const GridPosition& gridPosition)
			: id(id),
			gridPosition(gridPosition) {
		}

		// Constructor if the world position isn't 0,0
		Tile(const uint32_t& id,
			const GridPosition& gridPosition,
			const Maths::Position& worldPosition)
			: id(id),
			gridPosition(gridPosition),
			worldPosition(worldPosition) {
		}

		/** Returns the tile's coordinates within the grid. */
		GridPosition GetGridPosition() const;

		/** Returns the tile's unique identifier. */
		uint32_t GetId() const;

		/** Returns whether the tile blocks pathfinding/movement. */
		bool GetObstacle() const;

		/** Marks the tile as an obstacle or as walkable. */
		void SetObstacle(bool isObstacle);


	private:
		Maths::Position worldPosition = { 0.0f, 0.0f };

		uint32_t id = 0;
		GridPosition gridPosition = { 0, 0 };

		bool obstacle = false;
	};
}