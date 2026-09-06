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

    // Each Entity has an id. To make it unique, we use a generations system.
    // A destroyed Entity will just free the id slot (== make it available for
    // a new Entity). To make sure nothing references a destroyed element,
    // we increment generation every time a new Entity takes the slot.
    struct EntityId {
        uint32_t id = 0;
        uint32_t generation = 0;
    };

}