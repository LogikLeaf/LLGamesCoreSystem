#include "LL_RTS_Player.h"

namespace LL::RTS {
    
    void Player::SelectCharacter(Character* inCharacter) {
        selectedCharacter = inCharacter;
    }
    void Player::SelectTarget(Character* inTarget) {
        target = inTarget;
        target->SetFlag(Flag::Selected);
    }
    void Player::AttackTarget(uint32_t numberOfAttacks) {
        if (!target || !selectedCharacter) return;

        for (uint32_t i = 0; i < numberOfAttacks; i++) {
            selectedCharacter->Attack(target);
            if (target->HasFlag(Flag::Dead)) return;
        }
    }
    void Player::GiveOrder(Order order) {
        if (!selectedCharacter) return;
        selectedCharacter->AddOrder(order);
    }
    Character* Player::GetSelectedCharacter() { return selectedCharacter; }

}