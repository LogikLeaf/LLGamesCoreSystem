/*
* Everything happens in a 2D space. We use this grid to build the world
* in chunks for faster calculations.
*
* This grid can combine with many other systems. The pathfinder needs it for
* efficiency, and the GridSystem is completly dependant of it.
*/

#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "LL_Maths.h"
#include "LL_GridTile.h"



namespace LL {

	/** Owns and manages the tiles of a rectangular grid, and converts between grid and world coordinates. */
	class Grid {
	private:
		// ----- Constant variables -----
		// Sometimes the playable area isn't the full rendered area
		// (World origin can be at (0,0) but the playable space at (10, 10) for example)
		// This helps place the tiles where we want
		const Maths::Position GRID_WORLD_POSITION = { 0.0f, 0.0f };

		const uint32_t MAP_SIZE_X = 100;
		const uint32_t MAP_SIZE_Y = 100;

		const uint32_t NUMBER_OF_TILES = MAP_SIZE_X * MAP_SIZE_Y;
		// ------------------------------


	public:
		/**
		 * Creates a grid.
		 *
		 * @param gridWorldPosition World-space position of the grid's origin (tile 0,0).
		 * @param mapSizeX Number of tiles along the X axis.
		 * @param mapSizeY Number of tiles along the Y axis.
		 */
		Grid(const Maths::Position& gridWorldPosition,
			const uint32_t& mapSizeX,
			const uint32_t& mapSizeY)
			:
			GRID_WORLD_POSITION(gridWorldPosition),
			MAP_SIZE_X(mapSizeX),
			MAP_SIZE_Y(mapSizeY),
			NUMBER_OF_TILES(mapSizeX* mapSizeY) {
		}


		// There are two types of coordinates: grid coordinates and world coordinates
		// We need a conversion from one to another
		// ----- From grid to world -----
		/** Converts a grid X coordinate to a world X coordinate (tile center). */
		float GridXToWorldX(uint32_t xGrid) const;
		/** Converts a grid Y coordinate to a world Y coordinate (tile center). */
		float GridYToWorldY(uint32_t yGrid) const;
		// ------------------------------

		// ----- From world to grid -----
		/** Converts a world X coordinate to a grid X coordinate. */
		float WorldXToGridX(float xWorld) const;
		/** Converts a world Y coordinate to a grid Y coordinate. */
		float WorldYToGridY(float yWorld) const;
		// ------------------------------

		// Getters
		/** Returns the unique tile id for a given grid coordinate. */
		uint32_t GetTileIdFromGridCoordinates(uint32_t xLocation, uint32_t yLocation) const;
		/** Returns the grid Y coordinate corresponding to a tile id. */
		uint32_t GetYGridFromId(uint32_t tileId) const;
		/** Returns the grid X coordinate corresponding to a tile id. */
		uint32_t GetXGridFromId(uint32_t tileId) const;
		/** Returns the number of tiles along the X axis. */
		uint32_t GetMapSizeX() const;
		/** Returns the number of tiles along the Y axis. */
		uint32_t GetMapSizeY() const;
		/** Returns the tile at the given grid coordinates, or nullptr if out of bounds. */
		GridTile* GetTileAt(uint32_t x, uint32_t y);

		// Actions
		/** Creates and stores every tile of the grid. Must be called once before using the grid. */
		void SpawnTiles();


	private:
		// ----- Variables -----
		// All the tiles from the actual playing field
		std::vector<std::unique_ptr<GridTile>> tiles;
		// ---------------------
	};

}