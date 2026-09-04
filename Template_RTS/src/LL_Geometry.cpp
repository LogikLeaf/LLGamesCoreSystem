#include "LL_Geometry.h"
#include "LL_Maths.h"
#include <cmath>
#include <vector>
#include <algorithm>

namespace LL::Geometry {
	float Segment::Length() const {
		float dx = p2.x - p1.x;
		float dy = p2.y - p1.y;
		return std::sqrt(dx * dx + dy * dy);
	}


	Triangle::Triangle(Point A, Point B, Point C) {

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
		if (IsPointInQuad(this->A, otherQuad) || IsPointInQuad(this->B, otherQuad) || IsPointInQuad(this->C, otherQuad) || IsPointInQuad(this->D, otherQuad)) {
			return true;
		}
		// Check if any of otherQuad's corners are inside Q1
		if (IsPointInQuad(otherQuad.A, *this) || IsPointInQuad(otherQuad.B, *this) || IsPointInQuad(otherQuad.C, *this) || IsPointInQuad(otherQuad.D, *this)) {
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

	float whichSide(const Point& p1, const Point& p2, const Point& p3) {
		return (p1.x - p3.x) * (p2.y - p3.y)
			- (p2.x - p3.x) * (p1.y - p3.y);
	}

	bool isPointInTriangle(const Point& P, const Triangle& T) {
		const float EPS = 1e-6f; // Used to avoid issues where a float is unprecise and values "almost" 0 instead of 0

		float side1 = whichSide(P, T.A, T.B);
		float side2 = whichSide(P, T.B, T.C);
		float side3 = whichSide(P, T.C, T.A);

		bool hasNegative =
			(side1 < -EPS) || (side2 < -EPS) || (side3 < -EPS);

		bool hasPositive =
			(side1 > EPS) || (side2 > EPS) || (side3 > EPS);

		return !(hasNegative && hasPositive);
	}

	bool IsPointInQuad(const Point& P, const Quad& Q) {
		return  isPointInTriangle(P, Q.ABC) ||
			isPointInTriangle(P, Q.ACD);
	}

	Quad createQuad(Point center, uint32_t sizeX, uint32_t sizeY, uint32_t rotationDegrees) {
		std::vector<Point> corners(4);

		// 1. Convert the angle to radians
		float angleRad = rotationDegrees * M_PI / 180.0;

		// 2. Calculate the cosine and sine (only once for optimization)
		float cosTheta = cos(angleRad);
		float sinTheta = sin(angleRad);

		// 3. Half-dimensions (distance from the center to the edges)
		float halfWidth = sizeX / 2.0;
		float halfHeight = sizeY / 2.0;

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

	Quad createQuad(uint32_t x, uint32_t y, uint32_t sizeX, uint32_t sizeY, uint32_t rotationDegrees) {
		return createQuad(Point{ static_cast<float>(x), static_cast<float>(y) }, sizeX, sizeY, rotationDegrees);
	}


	Segment perpendicularAt(const Segment& base, float t, float length) {
		const float EPS = 1e-6f;
		t = Maths::Clamp01(t);

		// base vector
		float dx = base.p2.x - base.p1.x;
		float dy = base.p2.y - base.p1.y;
		float baseLen = std::sqrt(dx * dx + dy * dy);

		// crossing point on base
		Point cross = base.p1;
		if (baseLen > EPS) {
			float ux = dx / baseLen;
			float uy = dy / baseLen;
			float distAlong = t * baseLen;
			cross.x += ux * distAlong;
			cross.y += uy * distAlong;

			// perpendicular unit
			Point perp = { -uy, ux };

			float chosenLen = (length > 0.0f) ? length : baseLen;
			float half = chosenLen / 2.0f;

			Point pA = { cross.x + perp.x * half, cross.y + perp.y * half };
			Point pB = { cross.x - perp.x * half, cross.y - perp.y * half };
			return Segment{ pA, pB };
		}
		// base has (almost) zero length: create vertical segment centered on base.p1
		float chosenLen = (length > 0.0f) ? length : 1.0f;
		float half = chosenLen / 2.0f;
		Point pA = { base.p1.x, base.p1.y + half };
		Point pB = { base.p1.x, base.p1.y - half };
		return Segment{ pA, pB };
	}

	Segment perpendicularFrom(const Segment& base, float t, int directionSign, float length) {
		const float EPS = 1e-6f;
		t = Maths::Clamp01(t);
		int dir = (directionSign >= 0) ? 1 : -1;

		// base vector
		float dx = base.p2.x - base.p1.x;
		float dy = base.p2.y - base.p1.y;
		float baseLen = std::sqrt(dx * dx + dy * dy);

		// start point on base
		Point start = base.p1;
		if (baseLen > EPS) {
			float ux = dx / baseLen;
			float uy = dy / baseLen;
			float distAlong = t * baseLen;
			start.x += ux * distAlong;
			start.y += uy * distAlong;

			// perpendicular unit
			Point perp = { -uy, ux };

			float chosenLen = (length > 0.0f) ? length : baseLen;

			Point end = { start.x + perp.x * dir * chosenLen, start.y + perp.y * dir * chosenLen };
			return Segment{ start, end };
		}
		// base has (almost) zero length: start is base.p1, create vertical segment extending by chosenLen
		float chosenLen = (length > 0.0f) ? length : 1.0f;
		Point end = { base.p1.x, base.p1.y + dir * chosenLen };
		return Segment{ base.p1, end };
	}

}