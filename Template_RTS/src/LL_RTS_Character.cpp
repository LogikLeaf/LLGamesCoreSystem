#include "LL_RTS_Character.h"
#include "LL_RTS_GameMaster.h"
#include "LL_RTS_Player.h"
#include "LL_RTS_ResourcePool.h"
#include "LL_Maths.h"

namespace LL::RTS {

    // Default constructor (safe minimal implementation)
    Character::Character()
        : Entity(nullptr, Maths::Vector2D{ 0,0 }, EntityId()) {
    }

    // Make sure we initialize base Entity with the GM (use a default position/id here)
    Character::Character(GameMaster* GM) 
        : Entity(GM, Maths::Vector2D{0,0}, EntityId()) { }

    // Time
    void Character::Update(float deltaTime) {
        if (HasFlag(Flag::Dead)) return;
        
        // Decrement attack timer (runtime state)
        if (attackTimer > 0.0f) {
            attackTimer -= deltaTime;
            if (attackTimer < 0.0f) attackTimer = 0.0f;
        }

        ExecuteOrders();
        UpdateMovement(deltaTime);

        /* TODO: */
        // bool paid = owningPlayer->PayUnit(maintenanceCost); // I ask the player to pay for my maintenance, returns false if he couldn't
        /* 
        * if (!paid) TakeDamage( ... ); // I loose HP (decide how much, maybe 10%? ) if my maintenance cost was not paid this loop
        * 
        * Something to consider: we won't make this update at every tick, 
        * so we need a timer that tells how long we wait between each maintenance cost trigger.
        */
    }

    void Character::ExecuteOrders() {
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

             // Compute the desired anchor position (target position + per-unit offset stored in execution.destination)
            {
                Maths::Vector2D desired = {
                    execution.target->GetPosition().x + execution.destination.x,
                    execution.target->GetPosition().y + execution.destination.y
                };

                // Distance to desired anchor
                float distanceToAnchor = Maths::VectorLength(position, desired);

                // If within attack range (considering our range stat), stop chasing and attack
                if (distanceToAnchor <= static_cast<float>(range.current)) {
                    StopChase();

                    // Is cooldown over?
                    if (attackTimer <= 0.0f) {
                        Attack(execution.target);
                        
                        // Reset timer to enter cooldown
                        attackTimer = GetAttackCooldownsSeconds();
                    }

                    if (execution.target->HasFlag(Flag::Dead)) {
                        orders.erase(orders.begin());
                    }
                }
                else {
                    // Need to move toward the desired anchor. Start chase with stored offset if first time.
                    if (chaseTarget != execution.target) {
                        MoveTo(desired);
                        StartChase(execution.target, execution.destination);
                    }
                    else {
                        // Already chasing it, if we reached last chase point we execute a new MoveTo to keep following
                        if (destinations.empty()) MoveTo(desired);
                        // Otherwise do nothing because movement is in progress
                    }
                }
            }
            break;

        case OrderType::Stop:
            Stop();
            orders.clear();
            break;
        }

    }


    void Character::MoveToward(Maths::Vector2D target, float deltaTime) {
        direction = Maths::Direction(position, target);

        float distance = Maths::VectorLength(position, target);
        if (distance <= 0.0f) return;

        float movementDistance = movementSpeed.current * deltaTime;

        if (movementDistance >= distance) {
            position = target;
            return;
        }

        direction = Maths::Normalize(direction, distance);
        velocity = Maths::Velocity(
            direction,
            static_cast<float>(movementSpeed.current)
        );

        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;

        SetFlag(Flag::Moving);
    }

    void Character::UpdateMovement(float deltaTime) {

        if (chaseTarget && !chaseTarget->HasFlag(Flag::Dead)) {

            // Compute chase anchor as target position plus per-unit stored offset
            Maths::Vector2D chasePos = {
                chaseTarget->GetPosition().x + chaseOffset.x,
                chaseTarget->GetPosition().y + chaseOffset.y
            };

            // If we are now in range of the anchor, stop chase
            float distToAnchor = Maths::VectorLength(position, chasePos);
            if (distToAnchor <= static_cast<float>(range.current)) {
                StopChase();
                return;
            }

            MoveToward(chasePos, deltaTime);
            return;
        }

        if (destinations.empty()) {
            ClearFlag(Flag::Moving);
            return;
        }

        MoveToward(destinations.front(), deltaTime);

        if (position.x == destinations.front().x &&
            position.y == destinations.front().y) {

            destinations.erase(destinations.begin());

            if (destinations.empty())
                ClearFlag(Flag::Moving);
        }
    }


    bool Character::IsInRange(Character* inTarget) const {
        if (!inTarget) return false;

        float distance = Maths::VectorLength(
            position,
            inTarget->GetPosition()
        );

        return distance <= static_cast<float>(range.current);
    }

    void Character::StartChase(Character* target, Maths::Vector2D offset) { 
        chaseTarget = target; 
        chaseOffset = offset;
    }
    
    void Character::StopChase() {
        chaseTarget = nullptr;
        chaseOffset = { 0, 0 };
        destinations.clear();
        ClearFlag(Flag::Moving);
    }

    float Character::GetAttackCooldownsSeconds() const {
        if (attackSpeed.current == 0) return std::numeric_limits<float>::infinity();
        return 100.0f / static_cast<float>(attackSpeed.current); // seconds per attack
    }

    // Getters
    std::string Character::GetName() const { return name; }
    uint32_t Character::GetAttack() const { return attackDamage.current; }
    Maths::Vector2D Character::GetPosition() const { return position; }
    Maths::Vector2D Character::GetDirection() const { return direction; }
    Maths::Vector2D Character::GetVelocity() const { return velocity; }

    // Setters
    void Character::SetName(const std::string& inName) { name = inName; }

    // Orders
    void Character::AddOrder(Order order) {
        if (HasFlag(Flag::Dead)) return;
        orders.push_back(order);
    }


    // Actions
    void Character::TakeDamage(uint32_t damage) {
        Log(name + "'s health = " + std::to_string(health.current) + ".");

        Log(name + " receives " + std::to_string(damage) + " damage.");

        if (damage >= health.current) {
            health.current = 0;
            Kill();
        }
        else health.current -= damage;

        //GM->UpdateState(this); // Note: Need to think about this architecture

        Log(name + " has " + std::to_string(health.current) + " health left...");
    }

    void Character::Attack(Character* inTarget) {
        if (!inTarget) return;
        if (HasFlag(Flag::Dead) || inTarget->HasFlag(Flag::Dead)) return;

        Log(name + " attacks his target!");

        inTarget->TakeDamage(attackDamage.current);
    }


    void Character::Kill() {
        Stop();
        orders.clear();
        SetFlag(Flag::Dead);
        Log(name + " is dead.");
    }

    // ===== Movements =====
    // Go to a specified destination
    void Character::MoveTo(Maths::Vector2D inVector) {
        destinations.clear();
        destinations.push_back(inVector);
    }

    // Buffer multiple destinations
    void Character::AddDestination(Maths::Vector2D inDestination) {
        destinations.push_back(inDestination);
    }

    // Stop moving
    void Character::Stop() {
        destinations.clear();
        ClearFlag(Flag::Moving);
        StopChase();
    }


    void Character::LogPosition() {
        std::cout << name << " position is x = " << position.x
            << "; y = " << position.y << std::endl;
    }
    // =====================

    // Flags
    void Character::SetFlag(Flag flag) { flags |= static_cast<uint8_t>(flag); }
    void Character::ClearFlag(Flag flag) { flags &= ~static_cast<uint8_t>(flag); }
    bool Character::HasFlag(Flag flag) const { return (flags & static_cast<uint8_t>(flag)) != 0; }



}