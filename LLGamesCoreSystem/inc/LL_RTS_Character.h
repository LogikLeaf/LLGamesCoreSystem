#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <span>

#include "LL_RTS_Enumerators.h"
#include "LL_RTS_Structs.h"
#include "LL_RTS_Entity.h"
#include "LL_Movement.h"

// Global log for debug
template <typename T>
void Log(const T& msg) {
    std::cout << msg << std::endl;
}

namespace LL::RTS {

    class GameMaster;
    class Player;
    class ResourcePool;

    /** A controllable unit that executes orders, moves, and fights. */
    class Character : public Entity {

    public:

        Character();
        Character(GameMaster* GM);
        Character(GameMaster* GM, std::string name);

        /** Advances this character's state (orders, movement, cooldowns) by deltaTime seconds. */
        void Update(float deltaTime, Grid& grid,
            std::span<Movement::MovingAgent* const> neighbors);

    private:
        // Orders
        void ExecuteOrders();

        // Movements
        //void MoveToward(Maths::Position target, float deltaTime);
        //void UpdateMovement(float deltaTime);

        // Range detection for attack
        bool IsInRange(Character* target) const;

        // Chase
        void StartChase(Character* target, Maths::Position offset);
        void StopChase();

        // Cooldowns
        float GetAttackCooldownsSeconds() const;


    public:
        // Getters
        std::string GetName() const;
        uint32_t GetAttack() const;
        Stat GetHealth() const;
        Maths::Position GetPosition() const;
        Maths::Position GetDirection() const;
        Maths::Position GetVelocity() const;

        /** Non-owning access to this character's movement agent (position/velocity/bounds), for GameMaster snapshots. */
        Movement::MovingAgent& GetMovingAgent();

        // Setters
        /** Sets this character's display name. */
        void SetName(const std::string& inName);

        /** Queues an order to be executed after any pending orders. */
        void AddOrder(Order order);


        // Actions
        /** Reduces health by the given amount and kills the character if it reaches zero. */
        void TakeDamage(uint32_t damage);

        /** Deals this character's attack damage to target. */
        void Attack(Character* target);


        /** Marks this character as dead. */
        void Kill();

        // ===== Movements =====
        /** Clears the current path and (re)plans a route to position using the grid. */
        void MoveTo(Grid& grid, Maths::Position destination);

        ///** Queues an additional destination to move to after the current one. */
        //void AddDestination(Maths::Position inDestination);

        /** Stops movement and clears the current path. */
        void Stop();

        // Debug tool
        /** Logs the character's current position to the console. */
        void LogPosition();
        // =====================

        // Flags
        /** Sets the given state flag. */
        void SetFlag(Flag flag);
        /** Clears the given state flag. */
        void ClearFlag(Flag flag);
        /** Returns whether the given state flag is set. */
        bool HasFlag(Flag flag) const;


    protected:
        std::string name = "Character";

        Stat health = { 10,0,10,10 };
        Stat range = { 5,0,5,5 };
        Stat movementSpeed = { 5,0,5,5 };

        Stat attackDamage = { 3,0,3,3 };
        Stat attackSpeed = { 100,0,100,100 }; // 100 = 1 atk/second (means 100%)

        float attackTimer = 0.0f;

        uint8_t flags = 0;

        // How much of each resource this unit costs each tick?
        std::unique_ptr<ResourcePool> maintenanceCost;

        // Position/Velocity/Bounds live in the agent, consumed directly by MovementController
        Movement::MovingAgent agent;
        Maths::Position direction = { 0, 0 };

        Movement::MovementController movementController;

        // The Character will execute each one of these orders, one after the other
        std::vector<Order> orders;

        // Let's start the hunt
        Character* chaseTarget = nullptr;

        bool destinationPlanned = false;
        Maths::Position lastChaseAnchor = { 0,0 };

        // Per-unit chase offset - When chasing an attacked target keep formation
        Maths::Position chaseOffset = { 0, 0 };

        // Who owns me?
        // TODO: make it the AI Controller by default if no player controls it
        Player* owningPlayer = nullptr;


    };
}