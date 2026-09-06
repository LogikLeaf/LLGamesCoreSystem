#pragma once

#include <vector>
#include <string>
#include <iostream>

#include "LL_RTS_Enumerators.h"
#include "LL_RTS_Structs.h"
#include "LL_RTS_Entity.h"

// Global log for debug
template <typename T>
void Log(const T& msg) {
    std::cout << msg << std::endl;
}

namespace LL::RTS {

    class GameMaster;
    class Player;
    struct Vector2D;
    
    class Character : public Entity {

    public:

        Character(GameMaster* GM);

        // Time
        void Update(float deltaTime);

    private:
        // Orders
        void ExecuteOrders();

        // Movements
        void MoveToward(Maths::Vector2D target, float deltaTime);
        void UpdateMovement(float deltaTime);

        // Range detection for attack
        bool IsInRange(Character* inTarget) const;

        // Chase
        void StartChase(Character* target, Maths::Vector2D offset);
        void StopChase();

        // Cooldowns
        float GetAttackCooldownsSeconds() const;


    public:
        // Getters
        std::string GetName() const;
        uint32_t GetAttack() const;
        Stat GetHealth() const;
        Maths::Vector2D GetPosition() const;
        Maths::Vector2D GetDirection() const;
        Maths::Vector2D GetVelocity() const;

        // Setters
        void SetName(const std::string& inName);

        // Orders
        void AddOrder(Order order);


        // Actions
        void TakeDamage(uint32_t damage);

        void Attack(Character* inTarget);


        void Kill();

        // ===== Movements =====
        // Go to a specified destination
        void MoveTo(Maths::Vector2D inVector);

        // Buffer multiple destinations
        void AddDestination(Maths::Vector2D inDestination);

        // Stop moving
        void Stop();

        // Debug tool
        void LogPosition(); 
        // =====================

        // Flags
        void SetFlag(Flag flag);
        void ClearFlag(Flag flag);
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


        Maths::Vector2D position = { 0, 0 };
        Maths::Vector2D direction = { 0, 0 };
        Maths::Vector2D velocity = { 0, 0 };

        // Character's target destinations (we can buffer multiple destinations)
        std::vector<Maths::Vector2D> destinations;

        // The Character will execute each one of these orders, one after the other
        std::vector<Order> orders;

        // Let's start the hunt
        Character* chaseTarget = nullptr;

        // Per-unit chase offset - When chasing an attacked target keep formation
        Maths::Vector2D chaseOffset = { 0, 0 };

        // Who owns me?
        // TODO: make it the AI Controller by default if no player controls it
        Player* owningPlayer = nullptr; 


    };
}