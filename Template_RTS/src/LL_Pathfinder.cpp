#include "LL_Pathfinder.h"

#include <queue>
#include <unordered_map>
#include <cmath>
#include <limits>
#include <algorithm>

namespace LL::Pathfinder {

	// Internal helper utilities
	namespace {
		// A* search node tied to a grid cell.
		struct Node {
			int x = 0;
			int y = 0;
			float g = std::numeric_limits<float>::infinity(); ///< Cost from start to this node.
			float f = std::numeric_limits<float>::infinity(); ///< g + heuristic estimate to goal.
			int parentX = -1;
			int parentY = -1;
		};

		// Euclidean distance heuristic between two grid cells.
		inline float Heuristic(int x1, int y1, int x2, int y2) {
			float dx = float(x1 - x2);
			float dy = float(y1 - y2);
			return std::sqrt(dx * dx + dy * dy);
		}

		inline bool InBounds(Grid* grid, int x, int y) {
			return x >= 0 && y >= 0 && x < static_cast<int>(grid->GetMapSizeX()) && y < static_cast<int>(grid->GetMapSizeY());
		}

		// Convert grid coordinates to world position (center of tile)
		inline Maths::Position GridToWorld(Grid* grid, int gx, int gy) {
			Maths::Position p;
			p.x = grid->GridXToWorldX(static_cast<uint32_t>(gx));
			p.y = grid->GridYToWorldY(static_cast<uint32_t>(gy));
			return p;
		}

		// Convert world position to grid coords (round toward nearest int)
		inline void WorldToGrid(Grid* grid, const Maths::Position& w, int& outX, int& outY) {
			float gx = grid->WorldXToGridX(w.x);
			float gy = grid->WorldYToGridY(w.y);
			// assume each tile is 1 unit in world space (consistent with grid helpers)
			outX = static_cast<int>(std::floor(gx + 0.5f));
			outY = static_cast<int>(std::floor(gy + 0.5f));
		}

		// Bresenham-style line of sight check: returns false if any tile between
		// (x0,y0) and (x1,y1) is missing or is an obstacle.
		bool LineOfSight(Grid* grid, int x0, int y0, int x1, int y1) {
			int dx = std::abs(x1 - x0);
			int dy = std::abs(y1 - y0);
			int sx = (x0 < x1) ? 1 : -1;
			int sy = (y0 < y1) ? 1 : -1;
			int err = dx - dy;

			int x = x0;
			int y = y0;
			while (true) {
				auto tile = grid->GetTileAt(static_cast<uint32_t>(x), static_cast<uint32_t>(y));
				if (!tile || tile->GetObstacle()) return false;

				if (x == x1 && y == y1) break;

				int e2 = 2 * err;
				const bool stepX = e2 > -dy;
				const bool stepY = e2 < dx;

				if (stepX && stepY) {
					// Diagonal step: reject if either flanking orthogonal tile is blocked,
					// consistent with the neighbor-expansion corner-cut rule elsewhere.
					auto flankX = grid->GetTileAt(static_cast<uint32_t>(x + sx), static_cast<uint32_t>(y));
					auto flankY = grid->GetTileAt(static_cast<uint32_t>(x), static_cast<uint32_t>(y + sy));
					if ((flankX && flankX->GetObstacle()) || (flankY && flankY->GetObstacle())) {
						return false;
					}
				}

				if (stepX) { err -= dy; x += sx; }
				if (stepY) { err += dx; y += sy; }
			}
			return true;
		}

		// Walks parent links from the goal back to the start and returns the
		// resulting waypoints in start-to-goal order (world space, tile centers).
		std::vector<Maths::Position> ReconstructPath(Grid* grid, const std::vector<std::vector<Node>>& nodes, int sx, int sy, int ex, int ey) {
			std::vector<Maths::Position> pts;
			int cx = ex;
			int cy = ey;
			while (!(cx == sx && cy == sy)) {
				pts.push_back(GridToWorld(grid, cx, cy));
				const Node& n = nodes[cy][cx];
				if (n.parentX == -1 && n.parentY == -1) break;
				int px = n.parentX;
				int py = n.parentY;
				cx = px;
				cy = py;
			}
			pts.push_back(GridToWorld(grid, sx, sy));
			std::reverse(pts.begin(), pts.end());
			return pts;
		}
	}


	std::vector<Maths::Position> AStar::FindPath(Grid* grid, const Maths::Position& startWorld, const Maths::Position& goalWorld) {
		if (!grid) return {};

		int sx, sy, gx, gy;
		WorldToGrid(grid, startWorld, sx, sy);
		WorldToGrid(grid, goalWorld, gx, gy);

		if (!InBounds(grid, sx, sy) || !InBounds(grid, gx, gy)) return {};

		auto startTile = grid->GetTileAt(static_cast<uint32_t>(sx), static_cast<uint32_t>(sy));
		auto goalTile = grid->GetTileAt(static_cast<uint32_t>(gx), static_cast<uint32_t>(gy));
		if (!startTile || !goalTile) return {};
		if (startTile->GetObstacle() || goalTile->GetObstacle()) return {};

		int width = static_cast<int>(grid->GetMapSizeX());
		int height = static_cast<int>(grid->GetMapSizeY());

		// init node grid
		std::vector<std::vector<Node>> nodes(height, std::vector<Node>(width));
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				nodes[y][x].x = x;
				nodes[y][x].y = y;
			}
		}

		auto cmp = [](const Node* a, const Node* b) { return a->f > b->f; };
		std::priority_queue<Node*, std::vector<Node*>, decltype(cmp)> open(cmp);

		nodes[sy][sx].g = 0.0f;
		nodes[sy][sx].f = Heuristic(sx, sy, gx, gy);
		open.push(&nodes[sy][sx]);

		const int dxs[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
		const int dys[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };

		std::vector<std::vector<bool>> closed(height, std::vector<bool>(width, false));

		while (!open.empty()) {
			Node* current = open.top();
			open.pop();

			if (closed[current->y][current->x]) continue;
			closed[current->y][current->x] = true;

			if (current->x == gx && current->y == gy) {
				// path found
				return ReconstructPath(grid, nodes, sx, sy, gx, gy);
			}

			for (int i = 0; i < 8; ++i) {
				int nx = current->x + dxs[i];
				int ny = current->y + dys[i];
				if (!InBounds(grid, nx, ny)) continue;

				auto neighborTile = grid->GetTileAt(static_cast<uint32_t>(nx), static_cast<uint32_t>(ny));
				if (!neighborTile) continue;
				if (neighborTile->GetObstacle()) continue;

				// Diagonal moves are rejected if both orthogonal neighbors are blocked,
				// preventing the path from cutting through a corner.
				if (i % 2 == 1) {
					int ox = current->x + dxs[i];
					int oy = current->y;
					int ox2 = current->x;
					int oy2 = current->y + dys[i];
					auto t1 = grid->GetTileAt(static_cast<uint32_t>(ox), static_cast<uint32_t>(oy));
					auto t2 = grid->GetTileAt(static_cast<uint32_t>(ox2), static_cast<uint32_t>(oy2));
					if ((t1 && t1->GetObstacle()) || (t2 && t2->GetObstacle())) continue;
				}

				float moveCost = (i % 2 == 0) ? 1.0f : std::sqrt(2.0f);
				float tentativeG = current->g + moveCost;
				Node& nb = nodes[ny][nx];
				if (tentativeG < nb.g) {
					nb.g = tentativeG;
					nb.parentX = current->x;
					nb.parentY = current->y;
					nb.f = nb.g + Heuristic(nx, ny, gx, gy);
					open.push(&nb);
				}
			}
		}

		// no path
		return {};
	}


	std::vector<Maths::Position> AStar::FindPathNoDiagonal(Grid* grid, const Maths::Position& startWorld, const Maths::Position& goalWorld) {
		if (!grid) return {};

		int sx, sy, gx, gy;
		WorldToGrid(grid, startWorld, sx, sy);
		WorldToGrid(grid, goalWorld, gx, gy);

		if (!InBounds(grid, sx, sy) || !InBounds(grid, gx, gy)) return {};

		auto startTile = grid->GetTileAt(static_cast<uint32_t>(sx), static_cast<uint32_t>(sy));
		auto goalTile = grid->GetTileAt(static_cast<uint32_t>(gx), static_cast<uint32_t>(gy));
		if (!startTile || !goalTile) return {};
		if (startTile->GetObstacle() || goalTile->GetObstacle()) return {};

		int width = static_cast<int>(grid->GetMapSizeX());
		int height = static_cast<int>(grid->GetMapSizeY());

		// init node grid
		std::vector<std::vector<Node>> nodes(height, std::vector<Node>(width));
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				nodes[y][x].x = x;
				nodes[y][x].y = y;
			}
		}

		auto cmp = [](const Node* a, const Node* b) { return a->f > b->f; };
		std::priority_queue<Node*, std::vector<Node*>, decltype(cmp)> open(cmp);

		nodes[sy][sx].g = 0.0f;
		nodes[sy][sx].f = Heuristic(sx, sy, gx, gy);
		open.push(&nodes[sy][sx]);

		// 4-way movements: right, down, left, up
		const int dxs[4] = { 1, 0, -1, 0 };
		const int dys[4] = { 0, 1, 0, -1 };

		std::vector<std::vector<bool>> closed(height, std::vector<bool>(width, false));

		while (!open.empty()) {
			Node* current = open.top();
			open.pop();

			if (closed[current->y][current->x]) continue;
			closed[current->y][current->x] = true;

			if (current->x == gx && current->y == gy) {
				// path found
				return ReconstructPath(grid, nodes, sx, sy, gx, gy);
			}

			for (int i = 0; i < 4; ++i) {
				int nx = current->x + dxs[i];
				int ny = current->y + dys[i];
				if (!InBounds(grid, nx, ny)) continue;

				auto neighborTile = grid->GetTileAt(static_cast<uint32_t>(nx), static_cast<uint32_t>(ny));
				if (!neighborTile) continue;
				if (neighborTile->GetObstacle()) continue;

				float moveCost = 1.0f;
				float tentativeG = current->g + moveCost;
				Node& nb = nodes[ny][nx];
				if (tentativeG < nb.g) {
					nb.g = tentativeG;
					nb.parentX = current->x;
					nb.parentY = current->y;
					nb.f = nb.g + Heuristic(nx, ny, gx, gy);
					open.push(&nb);
				}
			}
		}

		// no path
		return {};
	}


	std::vector<Maths::Position> AnyAngle::FindPath(Grid* grid, const Maths::Position& startWorld, const Maths::Position& goalWorld) {
		if (!grid) return {};

		int sx, sy, gx, gy;
		WorldToGrid(grid, startWorld, sx, sy);
		WorldToGrid(grid, goalWorld, gx, gy);

		if (!InBounds(grid, sx, sy) || !InBounds(grid, gx, gy)) return {};

		auto startTile = grid->GetTileAt(static_cast<uint32_t>(sx), static_cast<uint32_t>(sy));
		auto goalTile = grid->GetTileAt(static_cast<uint32_t>(gx), static_cast<uint32_t>(gy));
		if (!startTile || !goalTile) return {};
		if (startTile->GetObstacle() || goalTile->GetObstacle()) return {};

		int width = static_cast<int>(grid->GetMapSizeX());
		int height = static_cast<int>(grid->GetMapSizeY());

		std::vector<std::vector<Node>> nodes(height, std::vector<Node>(width));
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				nodes[y][x].x = x;
				nodes[y][x].y = y;
			}
		}

		auto cmp = [](const Node* a, const Node* b) { return a->f > b->f; };
		std::priority_queue<Node*, std::vector<Node*>, decltype(cmp)> open(cmp);

		nodes[sy][sx].g = 0.0f;
		nodes[sy][sx].f = Heuristic(sx, sy, gx, gy);
		nodes[sy][sx].parentX = sx;
		nodes[sy][sx].parentY = sy;
		open.push(&nodes[sy][sx]);

		std::vector<std::vector<bool>> closed(height, std::vector<bool>(width, false));

		const int dxs[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
		const int dys[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };

		while (!open.empty()) {
			Node* current = open.top();
			open.pop();

			if (closed[current->y][current->x]) continue;
			closed[current->y][current->x] = true;

			if (current->x == gx && current->y == gy) {
				return ReconstructPath(grid, nodes, sx, sy, gx, gy);
			}

			for (int i = 0; i < 8; ++i) {
				int nx = current->x + dxs[i];
				int ny = current->y + dys[i];
				if (!InBounds(grid, nx, ny)) continue;

				auto neighborTile = grid->GetTileAt(static_cast<uint32_t>(nx), static_cast<uint32_t>(ny));
				if (!neighborTile) continue;
				if (neighborTile->GetObstacle()) continue;

				// diagonal corner-cut prevention (same as A*)
				if (i % 2 == 1) {
					int ox = current->x + dxs[i];
					int oy = current->y;
					int ox2 = current->x;
					int oy2 = current->y + dys[i];
					auto t1 = grid->GetTileAt(static_cast<uint32_t>(ox), static_cast<uint32_t>(oy));
					auto t2 = grid->GetTileAt(static_cast<uint32_t>(ox2), static_cast<uint32_t>(oy2));
					if ((t1 && t1->GetObstacle()) || (t2 && t2->GetObstacle())) continue;
				}

				Node& nb = nodes[ny][nx];

				// Theta* core idea: try to link the neighbor directly to current's
				// parent (skipping current) when a straight line of sight exists,
				// which avoids being locked to grid-aligned paths.
				int px = current->parentX;
				int py = current->parentY;
				float tentativeG = std::numeric_limits<float>::infinity();
				if (px != -1 && py != -1 && !(px == nx && py == ny)) {
					if (LineOfSight(grid, px, py, nx, ny)) {
						// cost from parent -> neighbor
						float costParentToNeighbor = Heuristic(px, py, nx, ny);
						float gFromParent = nodes[py][px].g + costParentToNeighbor;
						tentativeG = gFromParent;
						if (gFromParent < nb.g) {
							nb.parentX = px;
							nb.parentY = py;
						}
					}
				}

				// Fallback: no line of sight to the grandparent, so link normally to current.
				if (tentativeG == std::numeric_limits<float>::infinity()) {
					float cost = Heuristic(current->x, current->y, nx, ny);
					tentativeG = current->g + cost;
					if (tentativeG < nb.g) {
						nb.parentX = current->x;
						nb.parentY = current->y;
					}
				}

				if (tentativeG < nb.g) {
					nb.g = tentativeG;
					nb.f = nb.g + Heuristic(nx, ny, gx, gy);
					open.push(&nb);
				}
			}
		}

		return {};
	}

}