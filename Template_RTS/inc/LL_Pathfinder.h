#pragma once

#include <vector>

#include "LL_Maths.h"
#include "LL_Grid.h"

namespace LL::Pathfinder {

	/**
	 * A* pathfinder on a Grid.
	 * Positions are given and returned in world space; waypoints are tile centers.
	 */
	class AStar {
	public:
		/**
		 * Finds a path between two world positions using A* with 8-way movement (no corner cutting).
		 *
		 * @param grid Grid to search on.
		 * @param startWorld Start position in world space.
		 * @param goalWorld Goal position in world space.
		 * @return Ordered waypoints from start to goal, or an empty vector if no path exists.
		 */
		static std::vector<Maths::Position> FindPath(Grid::Grid* grid, const Maths::Position& startWorld, const Maths::Position& goalWorld);

		/** Same as FindPath but restricted to 4-way movement (no diagonals). */
		static std::vector<Maths::Position> FindPathNoDiagonal(Grid::Grid* grid, const Maths::Position& startWorld, const Maths::Position& goalWorld);
	};


	/**
	 * Any-angle pathfinder (Theta* style) producing more direct, non grid-locked paths.
	 */
	class AnyAngle {
	public:
		/**
		 * Finds a path between two world positions, allowing straight-line shortcuts when line of sight permits.
		 *
		 * @param grid Grid to search on.
		 * @param startWorld Start position in world space.
		 * @param goalWorld Goal position in world space.
		 * @return Ordered waypoints from start to goal, or an empty vector if no path exists.
		 */
		static std::vector<Maths::Position> FindPath(Grid::Grid* grid, const Maths::Position& startWorld, const Maths::Position& goalWorld);
	};

}