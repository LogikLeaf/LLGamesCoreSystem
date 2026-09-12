#pragma once

#include <cstdint>


namespace LL::RTS {
    /** Bitmask flags describing a Character's current state. */
    enum class Flag : uint8_t {
        Dead = 1 << 0,     ///< The character is dead.
        Selected = 1 << 1, ///< The character is currently selected by its owning player.
        Targeted = 1 << 2, ///< The character is targeted (e.g. by an attack order).
        Moving = 1 << 3    ///< The character is currently moving.
    };

    /** The kind of order given to a Character. */
    enum class OrderType {
        Move,   ///< Move to a destination.
        Attack, ///< Attack a target.
        Stop    ///< Stop the current order and clear the queue.
    };

    /** A type of resource a Player can own and spend. */
    enum class Resource : uint32_t {
        Wood,
        Stone,
        Gold,
        Food,

        Count = 4
    };
}