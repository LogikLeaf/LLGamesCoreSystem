#pragma once
#include <cstdint>
#include <string> // For test purposes



namespace LL::Maths {
    inline constexpr double PI = 3.14159265358979323846;

    // ===== Geometry functions =====

    /** A 2D point in world space. */
    struct Position {
        float x = 0.0f;
        float y = 0.0f;

        // Operators
        Position operator+(const Position& p) const;
        Position operator-(const Position& p) const;

        Position& operator*=(float scalar);
        Position& operator/=(float scalar);

        Position operator*(float scalar) const;
        Position operator/(float scalar) const;
    };

    Position operator*(float scalar, const Position& p);

    /** A directed segment between an origin and a target position. */
    struct Vector {
        // Essential variables
        Position origin = { 0, 0 };
        Position target = { 1, 1 };

        // Constructors
        // Keep default values
        Vector();
        /** Creates a unit-length vector starting at origin. */
        Vector(const Position& origin);
        /** Creates a vector between two explicit positions. */
        Vector(const Position& origin, const Position& target);


        /** Returns the distance between origin and target. */
        float Length() const;

        /** Returns the unit direction from origin to target. */
        Position Normalize() const;

        /** Returns a vector with the same origin and direction, scaled to the given length. */
        Vector operator*(float speed) const;
    };

    /**
     * Creates a vector perpendicular to `base`, centered on the point at parameter `t` (0..1) along `base`.
     *
     * @param t Position along the base vector where the perpendicular crosses it (0 = origin, 1 = target).
     * @param length Length of the resulting vector; uses the base vector's length if <= 0.
     */
    Vector PerpendicularVectorAt(const Vector& base, float t = 0.5f, float length = -1.0f);

    /**
     * Creates a vector perpendicular to `base`, starting at the point at parameter `t` (0..1) along `base`.
     *
     * @param t Position along the base vector where the perpendicular starts (0 = origin, 1 = target).
     * @param directionSign Direction of the perpendicular: +1 or -1.
     * @param length Length of the resulting vector; uses the base vector's length if <= 0.
     */
    Vector PerpendicularVectorFrom(const Vector& base, float t = 0.0f, int directionSign = +1, float length = -1.0f);


    /** A triangle defined by three positions. */
    struct Triangle {

        Triangle(Position A, Position B, Position C);

        Position A;
        Position B;
        Position C;

        Vector AB;
        Vector BC;
        Vector CA;

        /** Returns the triangle's area. */
        float Area() const;
    };


    /** A convex quadrilateral, represented internally as two triangles (ABC and ACD). Summit A is the "top left", the others follow clockwise. */
    struct Quad {

        // Constructors
        Quad() : A({ 0, 0 }), B({ 0, 0 }), C({ 0, 0 }), D({ 0, 0 }),
            ABC(A, B, C),
            ACD(A, C, D) {
        }
        Quad(Position A, Position B, Position C, Position D) : A(A), B(B), C(C), D(D),
            ABC(A, B, C),
            ACD(A, C, D) {
        }

        Position A;
        Position B;
        Position C;
        Position D;

        Triangle ABC;
        Triangle ACD;

        /** Returns whether this quad overlaps with another convex quad. */
        bool CollideWith(const Quad& otherQuad) const;

        /** Returns the smallest X coordinate among the quad's corners. */
        float GetMinX() const;
        /** Returns the largest X coordinate among the quad's corners. */
        float GetMaxX() const;
        /** Returns the smallest Y coordinate among the quad's corners. */
        float GetMinY() const;
        /** Returns the largest Y coordinate among the quad's corners. */
        float GetMaxY() const;

        /** Returns the quad's area. */
        float Area() const;

        // For log tests
        std::string ToString() const;
    };

    /** Returns which side of the line (p2,p3) the point p1 falls on: positive, negative, or ~0 if on the line. */
    float WhichSide(const Position& p1, const Position& p2, const Position& p3);

    /** Returns whether P lies inside triangle T. */
    bool IsPositionInTriangle(const Position& P, const Triangle& T);

    /** Returns whether P lies inside convex quad Q. Only works for convex quads. */
    bool IsPositionInQuad(const Position& P, const Quad& Q);

    /** Builds a quad of the given size, centered on `center` and rotated by `rotationDegrees`. */
    Quad CreateQuad(Position center, uint32_t sizeX, uint32_t sizeY, uint32_t rotationDegrees);
    /** Same as above, with the center given as separate integer coordinates. */
    Quad CreateQuad(uint32_t x, uint32_t y, uint32_t sizeX, uint32_t sizeY, uint32_t rotationDegrees);

    // ==============================



    // ===== Maths functions =====

    /** Clamps a value between 0 and 1. */
    float Clamp01(float t);


}