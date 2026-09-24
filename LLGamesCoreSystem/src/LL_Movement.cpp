#include "LL_Movement.h"
#include "LL_Pathfinder.h"

namespace LL::Movement {

    void PathFollower::SetPath(std::vector<Maths::Position> inWaypoints) {
        waypoints = std::move(inWaypoints);
        currentIndex = 0;
    }

    void PathFollower::Clear() {
        waypoints.clear();
        currentIndex = 0;
    }

    bool PathFollower::HasPath() const {
        return !waypoints.empty();
    }

    bool PathFollower::HasArrived() const {
        return waypoints.empty() || currentIndex >= waypoints.size();
    }

    Maths::Position PathFollower::GetCurrentTarget() const {
        if (HasArrived()) {
            return waypoints.empty() ? Maths::Position{} : waypoints.back();
        }
        return waypoints[currentIndex];
    }

    void PathFollower::AdvanceIfReached(const Maths::Position& currentPos, float reachRadius) {
        if (HasArrived()) {
            return;
        }
        const Maths::Vector toTarget(currentPos, waypoints[currentIndex]);
        if (toTarget.Length() <= reachRadius) {
            ++currentIndex;
        }
    }

    Maths::Position LocalAvoider::ComputeVelocity(
        const MovingAgent& self,
        const Maths::Position& desiredVelocity,
        std::span<MovingAgent* const> neighbors) const {

        Maths::Position separation{ 0.0f, 0.0f };

        for (const MovingAgent* other : neighbors) {
            if (other == &self) {
                continue;
            }

            const Maths::Vector between(other->position, self.position);
            const float distance = between.Length();
            const float combinedRadius = self.bounds.radius + other->bounds.radius;

            constexpr float safetyMargin = 0.25f;
            if (distance < combinedRadius + safetyMargin && distance > 0.0001f) {
                const float overlap = (combinedRadius + safetyMargin) - distance;
                separation = separation + between.Normalize() * overlap;
            }
        }

        return desiredVelocity + separation;
    }

    void MovementController::SetDestination(Grid& grid, const Maths::Position& fromPosition, const Maths::Position& worldGoal) {
        path.SetPath(Pathfinder::AnyAngle::FindPath(&grid, fromPosition, worldGoal));
    }

    void MovementController::Stop() {
        path.Clear();
    }

    bool MovementController::HasPath() const {
        return path.HasPath();
    }

    bool MovementController::HasArrived() const {
        return path.HasArrived();
    }

    void MovementController::Update(float deltaTime, MovingAgent& agent, float maxSpeed,
        std::span<MovingAgent* const> neighbors) {
        if (path.HasArrived()) {
            agent.velocity = { 0.0f, 0.0f };
            return;
        }

        const Maths::Position target = path.GetCurrentTarget();
        const Maths::Vector toTarget(agent.position, target);

        Maths::Position desired{ 0.0f, 0.0f };
        if (toTarget.Length() > 0.0001f) {
            desired = toTarget.Normalize() * maxSpeed;
        }

        agent.velocity = avoider.ComputeVelocity(agent, desired, neighbors);
        agent.position = agent.position + agent.velocity * deltaTime;

        path.AdvanceIfReached(agent.position, agent.bounds.radius);
    }

}