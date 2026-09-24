#include "LL_Maths.h"
#include <cmath>
#include <algorithm>
#include <cassert>
#include <string> // for test purposes

namespace LL::Maths {

    Position Position::operator+(const Position& p) const
    {
        return {
            x + p.x,
            y + p.y
        };
    }

    Position Position::operator-(const Position& p) const
    {
        return {
            x - p.x,
            y - p.y
        };
    }
    Position& Position::operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    Position& Position::operator/=(float scalar)
    {
        assert(scalar != 0.0f);
        x /= scalar;
        y /= scalar;
        return *this;
    }

    Position Position::operator*(float scalar) const
    {
        Position result = *this;
        result *= scalar;
        return result;
    }

    Position Position::operator/(float scalar) const
    {
        assert(scalar != 0.0f);
        Position result = *this;
        result /= scalar;
        return result;
    }


    // Keep default values
    Vector::Vector() {}

    // Set origin, target will be origin+1
    Vector::Vector(const Position& origin) : origin(origin) {}

    // Define everything
    Vector::Vector(const Position& origin, const Position& target) : origin(origin), target(target) {}


    float Vector::Length() const {
        float dx = target.x - origin.x;
        float dy = target.y - origin.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    Position Vector::Normalize() const {
        float length = Length();
        if (length == 0.0f) {
            return { 0.0f, 0.0f };
        }
        return {
            (target.x - origin.x) / length,
            (target.y - origin.y) / length
        };
    }


    Position operator*(float scalar, const Position& p) {
        return p * scalar; // Reuses Position::operator*(float)
    }

    Vector Vector::operator*(float speed) const {
        Position direction = Normalize(); // Vector's unit (target-origin)
        Position newTarget = origin + (direction * speed);
        return Vector(origin, newTarget);
    }


    Vector PerpendicularVectorAt(const Vector& base, float t, float length) {
        const float EPS = 1e-6f;
        t = Maths::Clamp01(t);

        // base vector
        float dx = base.target.x - base.origin.x;
        float dy = base.target.y - base.origin.y;
        float baseLen = std::sqrt(dx * dx + dy * dy);

        // crossing point on base
        Position cross = base.origin;
        if (baseLen > EPS) {
            float ux = dx / baseLen;
            float uy = dy / baseLen;
            float distAlong = t * baseLen;
            cross.x += ux * distAlong;
            cross.y += uy * distAlong;

            // perpendicular unit
            Position perp = { -uy, ux };

            float chosenLen = (length > 0.0f) ? length : baseLen;
            float half = chosenLen / 2.0f;

            Position pA = { cross.x + perp.x * half, cross.y + perp.y * half };
            Position pB = { cross.x - perp.x * half, cross.y - perp.y * half };
            return Vector{ pA, pB };
        }
        // base has (almost) zero length: create vertical segment centered on base.origin
        float chosenLen = (length > 0.0f) ? length : 1.0f;
        float half = chosenLen / 2.0f;
        Position pA = { base.origin.x, base.origin.y + half };
        Position pB = { base.origin.x, base.origin.y - half };
        return Vector{ pA, pB };
    }

    Vector PerpendicularVectorFrom(const Vector& base, float t, int directionSign, float length) {
        const float EPS = 1e-6f;
        t = Maths::Clamp01(t);
        int dir = (directionSign >= 0) ? 1 : -1;

        // base vector
        float dx = base.target.x - base.origin.x;
        float dy = base.target.y - base.origin.y;
        float baseLen = std::sqrt(dx * dx + dy * dy);

        // start point on base
        Position start = base.origin;
        if (baseLen > EPS) {
            float ux = dx / baseLen;
            float uy = dy / baseLen;
            float distAlong = t * baseLen;
            start.x += ux * distAlong;
            start.y += uy * distAlong;

            // perpendicular unit
            Position perp = { -uy, ux };

            float chosenLen = (length > 0.0f) ? length : baseLen;

            Position end = { start.x + perp.x * dir * chosenLen, start.y + perp.y * dir * chosenLen };
            return Vector{ start, end };
        }
        // base has (almost) zero length: start is base.origin, create vertical segment extending by chosenLen
        float chosenLen = (length > 0.0f) ? length : 1.0f;
        Position end = { base.origin.x, base.origin.y + dir * chosenLen };
        return Vector{ base.origin, end };
    }



    Triangle::Triangle(Position A, Position B, Position C) {
        this->A = A;
        this->B = B;
        this->C = C;

        AB = { A, B };
        BC = { B, C };
        CA = { C, A };
    }

    float Triangle::Area() const {
        float a = AB.Length();
        float b = BC.Length();
        float c = CA.Length();
        float s = (a + b + c) / 2.0f;
        return std::sqrt(s * (s - a) * (s - b) * (s - c));
    }


    float Quad::Area() const {
        return ABC.Area() + ACD.Area();
    }

    bool Quad::CollideWith(const Quad& otherQuad) const {
        // Check if any of Q1's corners are inside otherQuad
        if (IsPositionInQuad(this->A, otherQuad) || IsPositionInQuad(this->B, otherQuad) || IsPositionInQuad(this->C, otherQuad) || IsPositionInQuad(this->D, otherQuad)) {
            return true;
        }
        // Check if any of otherQuad's corners are inside Q1
        if (IsPositionInQuad(otherQuad.A, *this) || IsPositionInQuad(otherQuad.B, *this) || IsPositionInQuad(otherQuad.C, *this) || IsPositionInQuad(otherQuad.D, *this)) {
            return true;
        }
        return false;
    }

    float Quad::GetMinX() const {
        return std::min({ A.x, B.x, C.x, D.x });
    }
    float Quad::GetMaxX() const {
        return std::max({ A.x, B.x, C.x, D.x });
    }
    float Quad::GetMinY() const {
        return std::min({ A.y, B.y, C.y, D.y });
    }
    float Quad::GetMaxY() const {
        return std::max({ A.y, B.y, C.y, D.y });
    }

    std::string Quad::ToString() const {
        return "Quad(" + std::to_string(A.x) + "," + std::to_string(A.y) + " ; "
            + std::to_string(B.x) + "," + std::to_string(B.y) + " ; "
            + std::to_string(C.x) + "," + std::to_string(C.y) + " ; "
            + std::to_string(D.x) + "," + std::to_string(D.y) + ")";
    }


    float WhichSide(const Position& p1, const Position& p2, const Position& p3) {
        return (p1.x - p3.x) * (p2.y - p3.y)
            - (p2.x - p3.x) * (p1.y - p3.y);
    }

    bool IsPositionInTriangle(const Position& P, const Triangle& T) {
        const float EPS = 1e-6f; // Used to avoid issues where a float is unprecise and values "almost" 0 instead of 0

        float side1 = WhichSide(P, T.A, T.B);
        float side2 = WhichSide(P, T.B, T.C);
        float side3 = WhichSide(P, T.C, T.A);

        bool hasNegative =
            (side1 < -EPS) || (side2 < -EPS) || (side3 < -EPS);

        bool hasPositive =
            (side1 > EPS) || (side2 > EPS) || (side3 > EPS);

        return !(hasNegative && hasPositive);
    }

    bool IsPositionInQuad(const Position& P, const Quad& Q) {
        return  IsPositionInTriangle(P, Q.ABC) ||
            IsPositionInTriangle(P, Q.ACD);
    }

    Quad CreateQuad(Position center, uint32_t sizeX, uint32_t sizeY, uint32_t rotationDegrees) {
        Position corners[4];

        // 1. Convert the angle to radians
        float angleRad = static_cast<float>(rotationDegrees * PI / 180.0);

        // 2. Calculate the cosine and sine (only once for optimization)
        float cosTheta = cos(angleRad);
        float sinTheta = sin(angleRad);

        // 3. Half-dimensions (distance from the center to the edges)
        float halfWidth = sizeX / 2.0f;
        float halfHeight = sizeY / 2.0f;

        // 4. Calculate the 4 corners
        // Define the local, unrotated corners, then apply rotation + translation
        // Formula: 
        // x' = center.x + (x_local * cos - y_local * sin)
        // y' = center.y + (x_local * sin + y_local * cos)

        // Corner 1: Top-Left (-dx, -dy)
        corners[0].x = center.x + (-halfWidth * cosTheta - (-halfHeight) * sinTheta);
        corners[0].y = center.y + (-halfWidth * sinTheta + (-halfHeight) * cosTheta);

        // Corner 2 : Top-Right (dx, -dy)
        corners[1].x = center.x + (halfWidth * cosTheta - (-halfHeight) * sinTheta);
        corners[1].y = center.y + (halfWidth * sinTheta + (-halfHeight) * cosTheta);

        // Corner 3 : Bottom-Right (dx, dy)
        corners[2].x = center.x + (halfWidth * cosTheta - halfHeight * sinTheta);
        corners[2].y = center.y + (halfWidth * sinTheta + halfHeight * cosTheta);

        // Corner 4 : Bottom-Left (-dx, dy)
        corners[3].x = center.x + (-halfWidth * cosTheta - halfHeight * sinTheta);
        corners[3].y = center.y + (-halfWidth * sinTheta + halfHeight * cosTheta);

        return Quad(corners[0], corners[1], corners[2], corners[3]);
    }

    Quad CreateQuad(uint32_t x, uint32_t y, uint32_t sizeX, uint32_t sizeY, uint32_t rotationDegrees) {
        return CreateQuad(Position{ static_cast<float>(x), static_cast<float>(y) }, sizeX, sizeY, rotationDegrees);
    }
    // ============================


    // ===== Maths functions ===== 

    float Clamp01(float t) {
        if (t < 0.0f) return 0.0f;
        if (t > 1.0f) return 1.0f;
        return t;
    }


}