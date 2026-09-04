#pragma once
#include <string>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


namespace LL::Geometry {

    // Point in spaaaaace.
    struct Point {

		Point() : x(0), y(0) {}
		Point(float x, float y) : x(x), y(y) {}

        float x = 0.0f;
        float y = 0.0f;
    };

    // Line between two points.
    struct Segment {
        Point p1;
        Point p2;

        float Length() const;
    };

    // Creates a segment perpendicular to `base` that crosses it at parameter `t` (0..1).
    // The new segment is centered on the crossing point (endpoints extend equally).
    // If `length` <= 0 the base segment length is used.
    Segment PerpendicularSegmentAt(const Segment& base, float t = 0.5f, float length = -1.0f);
    
    // Creates a segment perpendicular to `base` whose one endpoint lies on the base at parameter `t` (0..1).
    // The other endpoint is placed in the perpendicular direction (directionSign = +1 or -1).
    // If `length` <= 0 the base segment length is used.
    Segment PerpendicularSegmentFrom(const Segment& base, float t = 0.0f, int directionSign = +1, float length = -1.0f);

    // Basic triangle.
    struct Triangle {

        Triangle(Point A, Point B, Point C);

        Point A;
        Point B;
        Point C;

        Segment AB;
        Segment BC;
        Segment CA;

        // Might be usefull later?
        float Area() const;
    };

	// Quadrilateral, represented as two triangles (ABC and ACD). The summit A is at the "top left" of the quad, and the other summits are ordered clockwise.
    struct Quad {

        // Constructeurs
		Quad() : A({ 0, 0 }), B({ 0, 0 }), C({ 0, 0 }), D({ 0, 0 }),
            ABC(A, B, C),
            ACD(A, C, D) {
		}
        Quad(Point A, Point B, Point C, Point D) : A(A), B(B), C(C), D(D),
            ABC(A, B, C),
            ACD(A, C, D) {
        }

        Point A;
        Point B;
        Point C;
        Point D;

        Triangle ABC;
        Triangle ACD;

        bool CollideWith(const Quad& otherQuad) const;

		float GetMinX() const;
		float GetMaxX() const;
		float GetMinY() const;
		float GetMaxY() const;

        // Might be usefull later?
        float Area() const;

        // For log tests
        std::string ToString() const;
    };

    // On which side of the line is our point placed?
    float WhichSide(const Point& p1, const Point& p2, const Point& p3);

    bool IsPointInTriangle(const Point& P, const Triangle& T);

    // Only works for convex quads!
    bool IsPointInQuad(const Point& P, const Quad& Q);

	Quad CreateQuad(Point center, uint32_t sizeX, uint32_t sizeY, uint32_t rotationDegrees);
	Quad CreateQuad(uint32_t x, uint32_t y, uint32_t sizeX, uint32_t sizeY, uint32_t rotationDegrees);
}