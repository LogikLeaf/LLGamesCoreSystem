#pragma once

#include <memory>

#include "LL_RTS_Character.h"


namespace LL::RTS {



    class Player {
    public:
        void SelectCharacter(Character* inCharacter);

        void SelectTarget(Character* inTarget);

        void AttackTarget(uint32_t numberOfAttacks);

        void GiveOrder(Order order);



        // Gettters
        Character* GetSelectedCharacter();

    private:
        Character* selectedCharacter = nullptr;
        Character* target = nullptr;
    };



}