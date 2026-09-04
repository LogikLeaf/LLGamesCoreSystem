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


        void SelectTarget(Character* inTarget);
        void AttackTarget(uint32_t numberOfAttacks);

        // Broadcast an order to all selected Characters
        void GiveOrder(Order order);



        // Gettters
        Character* GetSelectedCharacter(int index);
        const std::vector<Character*>& GetSelectedCharacters() const;

    private:
        std::vector<Character*> selectedCharacters;
        Character* target = nullptr;
    };



}