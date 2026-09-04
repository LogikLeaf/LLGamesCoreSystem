#pragma once

namespace LL::Maths {


    struct Vector2D {
        float x = 0;
        float y = 0;
    };

    // Helper function to clamp a value between 0 and 1
    float Clamp01(float t);

    // Vector related basic operations
    float VectorLength(Vector2D v1, Vector2D v2);
    Vector2D Direction(Vector2D v1, Vector2D v2);
    Vector2D Normalize(Vector2D vector, float length);
    Vector2D Velocity(Vector2D direction, float speed);
}