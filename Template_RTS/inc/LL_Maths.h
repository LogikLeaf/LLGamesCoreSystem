#pragma once
#include <cmath>

namespace LL::Maths {
    struct Vector2D {
        float x = 0;
        float y = 0;
    };


    float VectorLength(Vector2D v1, Vector2D v2);

    Vector2D Direction(Vector2D v1, Vector2D v2);

    Vector2D Normalize(Vector2D vector, float length);

    Vector2D Velocity(Vector2D direction, float speed);
}