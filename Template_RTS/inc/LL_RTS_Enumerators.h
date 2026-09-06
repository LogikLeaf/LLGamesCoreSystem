#pragma once

#include <cstdint>


namespace LL::RTS {
    enum class Flag : uint8_t {
        Dead = 1 << 0,
        Selected = 1 << 1,
        Targeted = 1 << 2,
        Moving = 1 << 3
    };


    enum class OrderType {
        Move,
        Attack,
        Stop
    };

    enum class Resource : uint32_t {
        Wood,
        Stone,
        Gold,
        Food,

        Count = 4
    };
}