#include "LL_RTS_Player.h"

namespace LL::RTS {
    
    void Player::SelectCharacter(Character* inCharacter) {
        // Clear current selection and their flags
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
        target = inTarget;
        target->SetFlag(Flag::Selected);
    }
    void Player::AttackTarget(uint32_t numberOfAttacks) {
        if (!target || selectedCharacters.empty()) return;

        for (uint32_t i = 0; i < numberOfAttacks; i++) {
            selectedCharacters.at(i)->Attack(target);
            if (target->HasFlag(Flag::Dead)) return;
        }
    }
    void Player::GiveOrder(Order order) {
        if (selectedCharacters.empty()) return;
        for (auto c : selectedCharacters) if (c) c->AddOrder(order);
    }
    Character* Player::GetSelectedCharacter(int index) { 
        if (selectedCharacters.empty() || 
            index >= selectedCharacters.size()) return nullptr;
        return selectedCharacters.at(index); 
    }

    const std::vector<Character*>& Player::GetSelectedCharacters() const {
        return selectedCharacters;
    }
}