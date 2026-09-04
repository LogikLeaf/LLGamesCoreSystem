#include "LL_Maths.h"

namespace LL::Maths {
    float VectorLength(Vector2D v1, Vector2D v2) {
        float dX = v2.x - v1.x;
        float dY = v2.y - v1.y;

        return static_cast<float>(std::sqrt(std::pow(dX, 2) + std::pow(dY, 2)));
    }

    Vector2D Direction(Vector2D v1, Vector2D v2) {
        return {
                v2.x - v1.x,
                v2.y - v1.y
        };
    }

    Vector2D Normalize(Vector2D vector, float length) {
        return {
                vector.x / length,
                vector.y / length
        };
    }

    Vector2D Velocity(Vector2D direction, float speed) {
        return {
            direction.x * speed,
            direction.y * speed
        };
    }
}