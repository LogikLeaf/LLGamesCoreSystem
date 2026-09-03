#include <iostream>
#include <stdint.h>
#include <memory>
#include <vector>
#include <string>
#include <cmath>


template <typename T>
void Log(const T& msg) {
    std::cout << msg << std::endl;
}


namespace RTS_Maths {
    struct Vector2D {
        float x = 0;
        float y = 0;
    };


    float VectorLength(Vector2D v1, Vector2D v2) {
        float dX = v2.x - v1.x;
        float dY = v2.y - v1.y;
         
        return static_cast<float>(std::sqrt(std::pow(dX, 2) + std::pow(dY, 2)));
    }


    Vector2D Direction(Vector2D v1, Vector2D v2) {
        return {
             v2.x - v1.x,
             v2.y - v1.y
        };
    }

    Vector2D Normalize(Vector2D vector, float length) {
        return {
             vector.x / length,
             vector.y / length
        };
    }

    Vector2D Velocity(Vector2D direction, float speed) {
        return {
            direction.x * speed,
            direction.y * speed
        };
    }

}

enum class Flag : uint8_t {
    Dead = 1 << 0,
    Selected = 1 << 1,
    Moving = 1 << 2
};



struct Stat {
    uint32_t base = 0;
    uint32_t modifier = 0;
    uint32_t current = 0;
    uint32_t maximum = 0;
};



enum class OrderType {
    Move,
    Attack,
    Stop
};

class Character;
struct Order {
    OrderType type;
    RTS_Maths::Vector2D destination;
    Character* target = nullptr;
};

class Character {

public:

    // Time
    void Update(float deltaTime) {
        if (HasFlag(Flag::Dead)) return;
        ExecuteOrders();
        UpdateMovement(deltaTime);
    }


private:
    // ===== Orders =====
    void ExecuteOrders() {
        if (orders.empty()) return;

        Order& execution = orders.front();

        switch (execution.type) {
        case OrderType::Move:
            MoveTo(execution.destination);

            if (position.x == execution.destination.x &&
                position.y == execution.destination.y) {

                orders.erase(orders.begin());
            }
            break;

        case OrderType::Attack:
            // Is Target invalid or already dead?
            if (!execution.target ||
                execution.target->HasFlag(Flag::Dead)) {

                // Stop chasing if target is dead or invalid
                StopChase();
                orders.erase(orders.begin());
                break;
            }

            // Target is in range, attack it
            if (IsInRange(execution.target)) {

                // We are in range, stop chasing and attack
                StopChase();
                Attack(execution.target);

                if (execution.target->HasFlag(Flag::Dead)) {
                    orders.erase(orders.begin());
                }
            }
            else {
                // Target is too far, so start chasing it if we are not already chasing it
                if (chaseTarget != execution.target) {
                    // First time starting to chase this target
                    MoveTo(execution.target->GetPosition());
                    StartChase(execution.target);
                }
                else {
                    // Already chasing it, if we reached last chase point we execute a new MoveTo to keep following
                    if (destinations.empty()) MoveTo(execution.target->GetPosition());
                    // Otherwise do nothing because movement is in progress
                }
            }
            break;

        case OrderType::Stop:
            Stop();
            StopChase();
            orders.clear();
            break;
        }

    }


    void UpdateMovement(float deltaTime) {
        // If we have a chase target, pursue its live position every frame
        if (chaseTarget && !chaseTarget->HasFlag(Flag::Dead)) {
            // If we're already in range, we shouldn't be chasing
            if (IsInRange(chaseTarget)) {
                StopChase();
                return;
            }

            // Compute live direction/velocity toward target
            RTS_Maths::Vector2D targetPos = chaseTarget->GetPosition();
            direction = RTS_Maths::Direction(position, targetPos);
            float distance = RTS_Maths::VectorLength(position, targetPos);
            if (distance <= 0.0f) return;

            // movement this frame
            float movementDistance = movementSpeed.current * deltaTime;
            if (movementDistance >= distance) {
                // arrive on top of target's last position (will loop next frame and update)
                position = targetPos;
                ClearFlag(Flag::Moving);
                return;
            }

            direction = RTS_Maths::Normalize(direction, distance);
            velocity = RTS_Maths::Velocity(direction, static_cast<float>(movementSpeed.current));
            if (!HasFlag(Flag::Moving)) SetFlag(Flag::Moving);
            position.x += velocity.x * deltaTime;
            position.y += velocity.y * deltaTime;
            return;
        }

        // Fallback: follow static destinations as before
        if (destinations.empty()) {
            ClearFlag(Flag::Moving);
            return;
        }

        if (!HasFlag(Flag::Moving)) SetFlag(Flag::Moving);
        RTS_Maths::Vector2D destination = destinations.front();
        direction = RTS_Maths::Direction(position, destination);
        float distance = RTS_Maths::VectorLength(position, destination);
        float movementDistance = movementSpeed.current * deltaTime;

        if (movementDistance >= distance) {
            position = destination;
            destinations.erase(destinations.begin());
            if (destinations.empty()) ClearFlag(Flag::Moving);
            return;
        }

        direction = RTS_Maths::Normalize(direction, distance);
        velocity = RTS_Maths::Velocity(direction, static_cast<float>(movementSpeed.current));
        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;
    }

    bool IsInRange(Character* inTarget) const {
        if (!inTarget) return false;
        
        float distance = RTS_Maths::VectorLength(
            position, 
            inTarget->GetPosition()
        );

        return distance <= static_cast<float>(range.current);
    }

    void StartChase(Character* target) { chaseTarget = target; }
    void StopChase() { 
        chaseTarget = nullptr;
        destinations.clear();
        ClearFlag(Flag::Moving);
    }


public:
    // Getters
    std::string GetName() const {return name;}
    uint32_t GetAttack() const { return attack.current; }
    RTS_Maths::Vector2D GetPosition() const { return position; }
    RTS_Maths::Vector2D GetDirection() const { return direction; }
    RTS_Maths::Vector2D GetVelocity() const { return velocity; }

    // Setters
    void SetName(const std::string& inName) { name = inName; }

    // Orders
    void AddOrder(Order order) {
        if (HasFlag(Flag::Dead)) return;
        orders.push_back(order);
    }


    // Actions
    void TakeDamage(uint32_t damage) {
            Log(name + "'s health = " + std::to_string(health.current) + ".");

        Log(name + " receives " + std::to_string(damage) + " damage.");

        if (damage >= health.current) {
            health.current = 0;
            Kill();
        } 
        else health.current -= damage;
        
        Log(name + " has " + std::to_string(health.current) + " health left...");
    }

    void Attack(Character* inTarget) {
        if (!inTarget) return;
        if (HasFlag(Flag::Dead) || inTarget->HasFlag(Flag::Dead)) return;

        Log(name + " attacks his target!");

        inTarget->TakeDamage(attack.current);
    }


    void Kill() {
        Stop();
        orders.clear();
        SetFlag(Flag::Dead);
        Log(name + " is dead.");
    }

    // ===== Movements =====
    // Go to a specified destination
    void MoveTo(RTS_Maths::Vector2D inVector) {
        destinations.clear();
        destinations.push_back(inVector);
    }

    // Buffer multiple destinations
    void AddDestination(RTS_Maths::Vector2D inDestination) {
        destinations.push_back(inDestination);
    }

    // Stop moving
    void Stop() { 
        destinations.clear();
        ClearFlag(Flag::Moving);
        StopChase();
    }


    void LogPosition() {
        std::cout   << name << " position is x = " << position.x
                    << "; y = " << position.y << std::endl;
    }
    // =====================

    // Flags
    void SetFlag(Flag flag) { flags |= static_cast<uint8_t>(flag); }
    void ClearFlag(Flag flag) { flags &= ~static_cast<uint8_t>(flag); }
    bool HasFlag(Flag flag) const { return (flags & static_cast<uint8_t>(flag)) != 0; }


protected:
    std::string name = "Character";

    Stat health = {10,0,10,10};
    Stat attack = {3,0,3,3};
    Stat range = {5,0,5,5};
    Stat movementSpeed = {5,0,5,5};

    uint8_t flags = 0;


    RTS_Maths::Vector2D position = {0, 0};
    RTS_Maths::Vector2D direction = {0, 0};
    RTS_Maths::Vector2D velocity = {0, 0};

    // Character's target destinations (we can buffer multiple destinations)
    std::vector<RTS_Maths::Vector2D> destinations;

    // The Character will execute each one of these orders, one after the other
    std::vector<Order> orders;

    // Let's start the hunt
    Character* chaseTarget = nullptr;


};



class Player {
public:
    void SelectCharacter(Character* inCharacter) {
        selectedCharacter = inCharacter;
    }

    void SelectTarget(Character* inTarget) {
        target = inTarget;
        target->SetFlag(Flag::Selected);
    }

    void AttackTarget(uint32_t numberOfAttacks) {
        if (!target || !selectedCharacter) return;

        for (uint32_t i = 0 ; i < numberOfAttacks ; i++ ) {
                selectedCharacter->Attack(target);
                if(target->HasFlag(Flag::Dead)) return;
        }
    }

    void GiveOrder(Order order) {
        if (!selectedCharacter) return;
        selectedCharacter->AddOrder(order);
    }



    // Gettters
    Character* GetSelectedCharacter() { return selectedCharacter; }

private:
    Character* selectedCharacter = nullptr;
    Character* target = nullptr;
};


int main() {
    Player player;
    auto bob = std::make_unique<Character>();
    bob->SetName("Bob");
    auto lili = std::make_unique<Character>();
    lili->SetName("Lili");

    player.SelectCharacter(bob.get());
    player.SelectTarget(lili.get());


    player.GiveOrder({
        OrderType::Move,
        {10, 0},
        nullptr
        });

    player.GiveOrder({
        OrderType::Move,
        {8, 12},
        nullptr
        });

    player.GiveOrder({
        OrderType::Attack,
        {0,0},
        lili.get()
        });

    lili->AddOrder({
        OrderType::Move,
        {30, 30},
        nullptr
        });

    for (int i = 0; i < 25; i++) {
        // Tick time
        lili->Update(1.0f);
        lili->LogPosition();

        bob->Update(1.0f);
        bob->LogPosition();
    }


    return 0;
}
