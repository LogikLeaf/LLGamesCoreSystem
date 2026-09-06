#include "LL_RTS_Player.h"
#include "LL_RTS_GameMaster.h"
#include "LL_Geometry.h"
#include "LL_Maths.h"
#include "LL_RTS_Character.h"
//#include "LL_RTS_ResourcePool.h"


#include <algorithm>
#include <cmath>


namespace LL::RTS {

    Player::Player(GameMaster* GM) : GM(GM), id(0) {}
    Player::Player(GameMaster* GM, uint32_t id) : GM(GM), id(id) {}




    void Player::SelectCharacter(Character* inCharacter) {
        // Also clears the flags
        ClearSelection();

        if (inCharacter) {
            selectedCharacters.push_back(inCharacter);
            inCharacter->SetFlag(Flag::Selected);
        }
    }

    void Player::AddToSelection(Character* inCharacter) {
        if (!inCharacter) return;
        auto it = std::find(selectedCharacters.begin(), selectedCharacters.end(), inCharacter);
        if (it == selectedCharacters.end()) {
            selectedCharacters.push_back(inCharacter);
            inCharacter->SetFlag(Flag::Selected);
        }
    }

    void Player::DeselectCharacter(Character* inCharacter) {
        if (!inCharacter) return;
        auto it = std::find(selectedCharacters.begin(), selectedCharacters.end(), inCharacter);
        if (it != selectedCharacters.end()) {
            (*it)->ClearFlag(Flag::Selected);
            selectedCharacters.erase(it);
        }
    }

    void Player::ClearSelection() {
        for (auto c : selectedCharacters) if (c) c->ClearFlag(Flag::Selected);
        selectedCharacters.clear();
    }


    void Player::SelectTarget(Character* inTarget) {
        if (!inTarget) return;
        // Clear previous flag if different
        if (target && (target != inTarget) ) target->ClearFlag(Flag::Targeted);
        target = inTarget;
        inTarget->SetFlag(Flag::Targeted);
    }

    void Player::DeselectTarget(Character* inTarget) {
        // TODO: Add logic for multiple players or units
        //       targetting another unit (do it inside
        //       Character)
        if (!target) return;

        if (target == inTarget) {
            target->ClearFlag(Flag::Targeted);
            target = nullptr;
        }
    }

    void Player::GiveOrder(Order order) {
        if (selectedCharacters.empty()) return;

        // There's only one unit, no extra calculation needed
        if (selectedCharacters.size() == 1) {
            for (auto c : selectedCharacters) if (c) c->AddOrder(order);
            return;
        }
        // For Move and Attack we compute a perpendicular formation line
        // Compute movement direction and perpendicular (world-space) using first unit as reference
        Geometry::Point startPoint;
        startPoint.x = selectedCharacters.front()->GetPosition().x;
        startPoint.y = selectedCharacters.front()->GetPosition().y;

        Maths::Vector2D start = { startPoint.x, startPoint.y };
        Maths::Vector2D dest = { order.destination.x, order.destination.y };

        // Direction from start to destination
        Maths::Vector2D dir = Maths::Direction(start, dest);
        float dirLen = Maths::VectorLength(start, dest);

        // If destination == start, choose a default direction
        if (dirLen <= 1e-6f) {
            dir = { 0.0f, 1.0f };
            dirLen = 1.0f;
        }

        // Perpendicular to dir: (-dy, dx)
        Maths::Vector2D perp = { -dir.y, dir.x };
        // normalize perp
        float perpLen = std::sqrt(perp.x * perp.x + perp.y * perp.y);
        if (perpLen <= 1e-6f) {
            perp = { 0.0f, 1.0f };
            perpLen = 1.0f;
        }
        perp = Maths::Normalize(perp, perpLen);

        size_t n = selectedCharacters.size();
        float half = (static_cast<float>(n) - 1.0f) / 2.0f;

        /* ============
        ** IF MOVEMENT
        ** ============ */

        // Handle movement orders: absolute destination per unit
        if (order.type == OrderType::Move) {
            Order modified = order;
            for (size_t i = 0; i < n; ++i) {
                if (!selectedCharacters[i]) continue;

                float indexOffset = (static_cast<float>(i) - half) * formationSpacing;
                modified.destination.x = order.destination.x + perp.x * indexOffset;
                modified.destination.y = order.destination.y + perp.y * indexOffset;

                selectedCharacters[i]->AddOrder(modified);
            }
            return;
        }

        /* ============
        ** IF ATTACK
        ** ============ */
        // Handle attack orders: store per-unit offset in Order.destination (relative to the target)
        if (order.type == OrderType::Attack) {
            Order modified = order;
            for (size_t i = 0; i < n; ++i) {
                if (!selectedCharacters[i]) continue;

                float indexOffset = (static_cast<float>(i) - half) * formationSpacing;
                // destination used as an offset relative to target position for attacks
                modified.destination.x = perp.x * indexOffset;
                modified.destination.y = perp.y * indexOffset;

                modified.target = order.target; // ensure target is kept
                selectedCharacters[i]->AddOrder(modified);
            }
            return;
        }


        // Other order types: broadcast unchanged
        for (auto c : selectedCharacters) if (c) c->AddOrder(order);
    }

    Character* Player::GetSelectedCharacter(size_t index) { 
        if (selectedCharacters.empty() || 
            index >= selectedCharacters.size()) return nullptr;
        return selectedCharacters.at(index); 
    }

    const std::vector<Character*>& Player::GetSelectedCharacters() const {
        return selectedCharacters;
    }


    bool Player::PayUnit(ResourcePool* resourceCost)
    {
        // 1. Check if we have enough of every resource
        for (size_t i = 0; i < static_cast<size_t>(Resource::Count); i++) {
            Resource resource = static_cast<Resource>(i);
            uint32_t cost = resourceCost->Get(resource);

            if (resources->Get(resource) < cost) return false; // We don't, stop here and return false

        }

        // 2. We have enough, proceed payout
        for (size_t i = 0; i < static_cast<size_t>(Resource::Count); i++) {
            Resource resource = static_cast<Resource>(i);
            uint32_t cost = resourceCost->Get(resource);
            resources->Adjust(resource, -static_cast<int64_t>(cost));
        }
        
        return true;

    }


    /*TODO: Building's maintenance cost
    * Just copy/paste the PayUnit method. Should be easy.
    */
    bool Player::PayBuilding(ResourcePool resourceCost)
    {
        return false;
    }



    bool Player::UseResource(Resource resource, uint32_t amount)
    {
        uint32_t current = resources->Get(resource);
        if (current < amount) return false; // Not enough, fail action and return false
        
        // Otherwise, proceed and return true
        resources->Adjust(resource, -static_cast<int64_t>(amount));
        return true;

    }
}