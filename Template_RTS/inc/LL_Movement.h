#pragma once

#include <vector>
#include <span>

#include "LL_Maths.h"
#include "LL_Grid.h"

namespace LL::Movement {

    /** Minimal view of a moving unit needed for local avoidance; not owned. */
    struct MovingAgent {
        Maths::Position position;
        Maths::Position velocity;
        Maths::CircleBounds bounds;
    };

    /** Walks a precomputed waypoint list, exposing the current steering target. */
    class PathFollower {
    public:
        void SetPath(std::vector<Maths::Position> waypoints);
        void Clear();

        bool HasPath() const;
        bool HasArrived() const;

        Maths::Position GetCurrentTarget() const;
        void AdvanceIfReached(const Maths::Position& currentPos, float reachRadius);

    private:
        std::vector<Maths::Position> waypoints;
        std::size_t currentIndex = 0;
    };

    /** Blends a desired velocity with separation from nearby agents to avoid overlap. */
    class LocalAvoider {
    public:
        /** Neighbors are non-owning pointers to agents managed elsewhere (e.g. by GameMaster's per-tick snapshot). */
        Maths::Position ComputeVelocity(
            const MovingAgent& self,
            const Maths::Position& desiredVelocity,
            std::span<MovingAgent* const> neighbors) const;
    };

    /**
     * Drives a single unit's movement: pathfinding-derived waypoints plus
     * local avoidance against neighboring agents.
     */
    class MovementController {
    public:
        void SetDestination(Grid& grid, const Maths::Position& fromPosition, const Maths::Position& worldGoal);

        void Stop();
        bool HasPath() const;
        bool HasArrived() const;

        void Update(float deltaTime, MovingAgent& agent, float maxSpeed,
            std::span<MovingAgent* const> neighbors);

    private:
        PathFollower path;
        LocalAvoider avoider;
    };

}