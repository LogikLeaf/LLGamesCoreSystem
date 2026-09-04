#pragma once

#include <vector>
#include <memory>

#include "LL_RTS_Character.h"


namespace LL::RTS {



    class Player {
    public:
        // Select a single Character
        void SelectCharacter(Character* inCharacter);

        // Add/Remove Characters to selection
        void AddToSelection(Character* inCharacter);
        void DeselectCharacter(Character* inCharacter);
        void ClearSelection();

        // Targeting
        void SelectTarget(Character* inTarget);
        void DeselectTarget(Character* inTarget);

        // Broadcast an order to all selected Characters
        void GiveOrder(Order order);

        // Gettters
        Character* GetSelectedCharacter(size_t index);
        const std::vector<Character*>& GetSelectedCharacters() const;

    private:
        std::vector<Character*> selectedCharacters;
        Character* target = nullptr;

        // The space between two units when moving in formation
        float formationSpacing = 3.0f;
    };



}