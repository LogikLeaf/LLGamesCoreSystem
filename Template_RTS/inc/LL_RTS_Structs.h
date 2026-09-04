#pragma once

#include "LL_RTS_Enumerators.h"
#include "LL_Maths.h"


namespace LL::RTS {
    struct Stat {
        uint32_t base = 0;
        uint32_t modifier = 0;
        uint32_t current = 0;
        uint32_t maximum = 0;
    };




    class Character;
    struct Order {
        OrderType type;
        Maths::Vector2D destination;
        Character* target = nullptr;
    };
}