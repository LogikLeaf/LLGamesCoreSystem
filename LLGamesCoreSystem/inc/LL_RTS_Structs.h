#pragma once

#include "LL_RTS_Enumerators.h"
#include "LL_Maths.h"


namespace LL::RTS {
    /** A numeric stat with a base value, an active modifier, and its resulting current/maximum values. */
    struct Stat {
        uint32_t base = 0;
        uint32_t modifier = 0;
        uint32_t current = 0;
        uint32_t maximum = 0;
    };


    class Character;

    /**
     * A command given to a Character.
     *
     * For Move orders, destination is the position to move to.
     * For Attack orders, destination is used as a per-unit formation offset from the target's position, and target is the character to attack.
     */
    struct Order {
        OrderType type;
        Maths::Position destination;
        Character* target = nullptr;
    };

    /**
     * Unique identifier for an Entity.
     *
     * generation is incremented each time a slot is reused, so stale references to a destroyed entity can be detected.
     */
    struct EntityId {
        uint32_t id = 0;
        uint32_t generation = 0;
    };

}