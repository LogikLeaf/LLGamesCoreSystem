#include "LL_RTS_Character.h"
#include "LL_RTS_GameMaster.h"
#include "LL_RTS_Player.h"
#include "LL_RTS_ResourcePool.h"
#include "LL_Maths.h"

namespace LL::RTS {

    // Default constructor (safe minimal implementation)
    Character::Character()
        : Entity(nullptr, EntityId()) {
    }

    // Make sure we initialize base Entity with the GM (use a default position/id here)
    Character::Character(GameMaster* GM) 
        : Entity(GM, EntityId()) { }

    Character::Character(GameMaster* GM, std::string name) 
        : Entity(GM, EntityId()), name(name) { }

    void Character::Update(float deltaTime, Grid& grid,
        std::span<Movement::MovingAgent* const> neighbors) {
        if (HasFlag(Flag::Dead)) return;

        if (attackTimer > 0.0f) {
            attackTimer -= deltaTime;
            if (attackTimer < 0.0f) attackTimer = 0.0f;
        }

        ExecuteOrders();

        if (chaseTarget && !chaseTarget->HasFlag(Flag::Dead)) {
            Maths::Position chasePos = {
                chaseTarget->GetPosition().x + chaseOffset.x,
                chaseTarget->GetPosition().y + chaseOffset.y
            };

            constexpr float replanThreshold = 1.0f;
            if (Maths::Vector(lastChaseAnchor, chasePos).Length() > replanThreshold) {
                movementController.SetDestination(grid, agent.position, chasePos);
                lastChaseAnchor = chasePos;
            }
        }

        movementController.Update(deltaTime, agent, static_cast<float>(movementSpeed.current), neighbors);

        if (agent.velocity.x != 0.0f || agent.velocity.y != 0.0f) {
            direction = Maths::Vector({ 0,0 }, agent.velocity).Normalize();
            SetFlag(Flag::Moving);
        }
        else ClearFlag(Flag::Moving);

        /* TODO: */
        // bool paid = owningPlayer->PayUnit(maintenanceCost); // I ask the player to pay for my maintenance, returns false if he couldn't
        /* 
        * if (!paid) TakeDamage( ... ); // I loose HP (decide how much, maybe 10%? ) if my maintenance cost was not paid this loop
        * 
        * Something to consider: we won't make this update at every tick, 
        * so we need a timer that tells how long we wait between each maintenance cost trigger.
        */
    }

    // ExecuteOrders
    void Character::ExecuteOrders() {
        if (orders.empty()) return;

        Order& execution = orders.front();

        switch (execution.type) {
        case OrderType::Move:
            if (!destinationPlanned && GM) {
                movementController.SetDestination(GM->GetGrid(), agent.position, execution.destination);
                destinationPlanned = true;
            }
            if (movementController.HasArrived()) {
                orders.erase(orders.begin());
                destinationPlanned = false;
            }
            break;

        case OrderType::Attack: {
            if (!execution.target || execution.target->HasFlag(Flag::Dead)) {
                StopChase();
                orders.erase(orders.begin());
                destinationPlanned = false;
                break;
            }

            Maths::Position desired = {
                execution.target->GetPosition().x + execution.destination.x,
                execution.target->GetPosition().y + execution.destination.y
            };

            float distanceToAnchor = Maths::Vector(agent.position, desired).Length();

            if (distanceToAnchor <= static_cast<float>(range.current)) {
                StopChase();

                if (attackTimer <= 0.0f) {
                    Attack(execution.target);
                    attackTimer = GetAttackCooldownsSeconds();
                }

                if (execution.target->HasFlag(Flag::Dead)) {
                    orders.erase(orders.begin());
                    destinationPlanned = false;
                }
            }
            else if (chaseTarget != execution.target) {
                StartChase(execution.target, execution.destination);
            }
            // else: already chasing, MovementController keeps re-targeting each Update via StartChase's retarget (see below).
            break;
        }

        case OrderType::Stop:
            Stop();
            orders.clear();
            destinationPlanned = false;
            break;
        }
    }



    /*void Character::MoveToward(Maths::Position target, float deltaTime) {
        Maths::Vector toTarget(position, target);
        float distance = toTarget.Length();
        if (distance <= 0.0f) return;

        float movementDistance = movementSpeed.current * deltaTime;

        if (movementDistance >= distance) {
            position = target;
            return;
        }

        // The Vector already models the direction; no separate helper needed.
        direction = toTarget.Normalize();
        velocity = direction * static_cast<float>(movementSpeed.current);

        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;

        SetFlag(Flag::Moving);
    }


    void Character::UpdateMovement(float deltaTime) {

        if (chaseTarget && !chaseTarget->HasFlag(Flag::Dead)) {

            // Compute chase anchor as target position plus per-unit stored offset
            Maths::Position chasePos = {
                chaseTarget->GetPosition().x + chaseOffset.x,
                chaseTarget->GetPosition().y + chaseOffset.y
            };

            // If we are now in range of the anchor, stop chase
            float distToAnchor = Maths::Vector(position, chasePos).Length();
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
    }*/


    bool Character::IsInRange(Character* target) const {
        if (!target) return false;

        const float distance = Maths::Vector(agent.position, target->GetPosition()).Length();
        return distance <= static_cast<float>(range.current);
    }

    void Character::StartChase(Character* target, Maths::Position offset) { 
        chaseTarget = target; 
        chaseOffset = offset;
        lastChaseAnchor = { 0,0 };
    }
    
    void Character::StopChase() {
        chaseTarget = nullptr;
        chaseOffset = { 0, 0 };
    }

    float Character::GetAttackCooldownsSeconds() const {
        if (attackSpeed.current == 0) return std::numeric_limits<float>::infinity();
        return 100.0f / static_cast<float>(attackSpeed.current); // seconds per attack
    }

    // Getters
    std::string Character::GetName() const              { return name; }
    uint32_t Character::GetAttack() const               { return attackDamage.current; }
    Maths::Position Character::GetPosition() const      { return agent.position; }
    Maths::Position Character::GetVelocity() const      { return agent.velocity; }
    Maths::Position Character::GetDirection() const     { return direction; }
    Movement::MovingAgent& Character::GetMovingAgent()  { return agent; }

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

    void Character::Attack(Character* target) {
        if (!target) return;
        if (HasFlag(Flag::Dead) || target->HasFlag(Flag::Dead)) return;

        Log(name + " attacks his target!");

        target->TakeDamage(attackDamage.current);
    }


    void Character::Kill() {
        Stop();
        orders.clear();
        SetFlag(Flag::Dead);
        Log(name + " is dead.");
    }

    // ===== Movements =====
    // Go to a specified destination
    void Character::MoveTo(Grid& grid, Maths::Position destination) {
        movementController.SetDestination(grid, agent.position, destination);
        destinationPlanned = false;
    }

    /*// Buffer multiple destinations
    void Character::AddDestination(Maths::Position inDestination) {
        destinations.push_back(inDestination);
    }*/

    // Stop moving
    void Character::Stop() {
        movementController.Stop();
        StopChase();
        destinationPlanned = false;
        ClearFlag(Flag::Moving);
    }


    void Character::LogPosition() {
        std::cout << name << " position is x = " << agent.position.x
            << "; y = " << agent.position.y << std::endl;
    }
    // =====================

    // Flags
    void Character::SetFlag(Flag flag) { flags |= static_cast<uint8_t>(flag); }
    void Character::ClearFlag(Flag flag) { flags &= ~static_cast<uint8_t>(flag); }
    bool Character::HasFlag(Flag flag) const { return (flags & static_cast<uint8_t>(flag)) != 0; }



}