// Copyright (C) 2017-2018 Alexandre-Xavier Labonté-Lamoureux
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// vecmath.cpp
// Custom library for vector math, collision with planes, etc. that uses floats.

#include "vecmath.h"

#include <vector>
#include <cmath>	// sqrt
#include <limits>	// numeric_limits<float>::quiet_NaN()
using namespace std;

bool operator==(const Float3& lhs, const Float3& rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

// Ray-casting algorithm used to find if a 2D coordinate is on a 3D polygon
bool pointInPolyXY(const float x, const float y, const vector<Float3>& vertices)
{
	bool inside = false;
	// Iterate over every edge. Trace an infinite ray starting from the point.
	// If the number of intersections if even, it's outside. If it's odd, the point is inside.
	for (unsigned int i = 0, j = vertices.size() - 1; i < vertices.size(); j = i++) {
		// Create new variables for readability
		float xi = vertices[i].x;
		float yi = vertices[i].y;
		float xj = vertices[j].x;
		float yj = vertices[j].y;

		bool intersect = ((yi > y) != (yj > y)) && (x < (xj - xi) * (y - yi) / (yj - yi) + xi);
		if (intersect)
			inside = !inside;
	}

	return inside;
}

bool pointInPolyYZ(const float x, const float y, const vector<Float3>& vertices)
{
	bool inside = false;
	// Iterate over every edge. Trace an infinite ray starting from the point.
	// If the number of intersections if even, it's outside. If it's odd, the point is inside.
	for (unsigned int i = 0, j = vertices.size() - 1; i < vertices.size(); j = i++) {
		// Create new variables for readability
		float xi = vertices[i].y;
		float yi = vertices[i].z;
		float xj = vertices[j].y;
		float yj = vertices[j].z;

		bool intersect = ((yi > y) != (yj > y)) && (x < (xj - xi) * (y - yi) / (yj - yi) + xi);
		if (intersect)
			inside = !inside;
	}

	return inside;
}

bool pointInPolyZX(const float x, const float y, const vector<Float3>& vertices)
{
	bool inside = false;
	// Iterate over every edge. Trace an infinite ray starting from the point.
	// If the number of intersections if even, it's outside. If it's odd, the point is inside.
	for (unsigned int i = 0, j = vertices.size() - 1; i < vertices.size(); j = i++) {
		// Create new variables for readability
		float xi = vertices[i].z;
		float yi = vertices[i].x;
		float xj = vertices[j].z;
		float yj = vertices[j].x;

		bool intersect = ((yi > y) != (yj > y)) && (x < (xj - xi) * (y - yi) / (yj - yi) + xi);
		if (intersect)
			inside = !inside;
	}

	return inside;
}

bool allEqualX(const vector<Float3>& vertices)
{
	for (unsigned int i = 0, j = vertices.size() - 1; i < vertices.size(); j = i++)
		if (!(vertices[i].x == vertices[j].x))
			return false;

	return true;
}

bool allEqualY(const vector<Float3>& vertices)
{
	for (unsigned int i = 0, j = vertices.size() - 1; i < vertices.size(); j = i++)
		if (!(vertices[i].y == vertices[j].y))
			return false;

	return true;
}

bool allEqualZ(const vector<Float3>& vertices)
{
	for (unsigned int i = 0, j = vertices.size() - 1; i < vertices.size(); j = i++)
		if (!(vertices[i].z == vertices[j].z))
			return false;

	return true;
}

Float3 crossProduct(const Float3& u, const Float3& v)
{
	return {u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x};
}

Float3 addVectors(const Float3& u, const Float3& v)
{
	return {u.x + v.x, u.y + v.y, u.z + v.z};
}

Float3 subVectors(const Float3& u, const Float3& v)
{
	return {u.x - v.x, u.y - v.y, u.z - v.z};
}

Float3 scaleVector(const float l, const Float3& v)
{
	return {l * v.x, l * v.y, l * v.z};
}

// Returns a normalized copy the passed vector
Float3 normalize(Float3 v)
{
	float len = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);

	if (len != 0.0f)
	{
		v.x /= len;
		v.y /= len;
		v.z /= len;
	}

	return v;
}

float dotProduct(const Float3& u, const Float3& v)
{
	return u.x * v.x + u.y * v.y + u.z * v.z;
}

// Parameters: ray, origin of ray, polygon's normal, center of polygon. The epsilon is optional. 'normal' and 'ray' must be normalized.
Float3 RayIntersect(const Float3& ray, const Float3& origin, const Float3& normal, const Float3& center, const float epsilon)
{
	if (fabs(dotProduct(normal, ray)) <= epsilon)	// No intersection, the line is parallel to the plane
		return {numeric_limits<float>::quiet_NaN(), numeric_limits<float>::quiet_NaN(), numeric_limits<float>::quiet_NaN()};

	// Get the denominator value (which is a scalar)
	float d = dotProduct(normal, center);

	// Compute the distance for the directed line ray intersecting the plane
	float x = (d - dotProduct(normal, origin)) / dotProduct(normal, ray);

	// Return the contact point
	return addVectors(origin, scaleVector(x, ray));
}

// Returns a normalized normal
Float3 ComputeNormal(const vector<Float3>& vertices)
{
	// Vector 'u'
	Float3 u = {vertices[1].x - vertices[0].x, vertices[1].y - vertices[0].y, vertices[1].z - vertices[0].z};

	// Vector 'v'
	Float3 v = {vertices[2].x - vertices[0].x, vertices[2].y - vertices[0].y, vertices[2].z - vertices[0].z};

	// Cross product to get the normal
	return normalize(crossProduct(u, v));
}

// Can compute the center of a polygon (its centroid) by doing an average of all of its vertices
Float3 ComputeAverage(const vector<Float3>& vertices)
{
	// Center of polygon
	Float3 total = {0, 0, 0};
	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		total.x += vertices[i].x;
		total.y += vertices[i].y;
		total.z += vertices[i].z;
	}
	return {total.x / vertices.size(), total.y / vertices.size(), total.z / vertices.size()};
}

// TODO: This function could be removed entirely or moved to "physics.cpp"
// TODO: return a Float3 or use "RayIntersect" directly
// Takes a X, Y, Z position to test with the polygon's normal and centroid
float PointHeightOnPoly(const float x, const float y, const float z, const Float3& normal, const Float3& centroid)
{
	// Trace a ray that aims down and return the height of its intersection
	return RayIntersect({0, 0, -1}, {x, y, z}, normal, centroid).z;
}

bool CheckVectorIntersection(const Float3& v1start, const Float3& v1end, const Float3& v2start, const Float3& v2end)
{
	// Cramer's rule. Reference: https://stackoverflow.com/a/1968345
	float WallDiffX = v1end.x - v1start.x;    // Vector's X from (0,0)
	float WallDiffY = v1end.y - v1start.y;    // Vector's Y from (0,0)
	float VectorWallOrthDiffX = v2end.x - v2start.x;
	float VectorWallOrthDiffY = v2end.y - v2start.y;

	float Denominator = -VectorWallOrthDiffX * WallDiffY + WallDiffX * VectorWallOrthDiffY;
	float PointWall = (-WallDiffY * (v1start.x - v2start.x) + WallDiffX * (v1start.y - v2start.y)) / Denominator;
	float PointVectorOrth = (VectorWallOrthDiffX * (v1start.y - v2start.y) - VectorWallOrthDiffY * (v1start.x - v2start.x)) / Denominator;

	// Check if a collision is detected (Not checking if touching an endpoint)
	return PointWall >= 0 && PointWall <= 1 && PointVectorOrth >= 0 && PointVectorOrth <= 1;
}

Float2 CollisionPoint(const Float3& v1start, const Float3& v1end, const Float3& v2start, const Float3& v2end)
{
	// Cramer's rule. Reference: https://stackoverflow.com/a/1968345
	float WallDiffX = v1end.x - v1start.x;    // Vector's X from (0,0)
	float WallDiffY = v1end.y - v1start.y;    // Vector's Y from (0,0)
	float VectorWallOrthDiffX = v2end.x - v2start.x;
	float VectorWallOrthDiffY = v2end.y - v2start.y;

	float Denominator = -VectorWallOrthDiffX * WallDiffY + WallDiffX * VectorWallOrthDiffY;
	float PointVectorOrth = (VectorWallOrthDiffX * (v1start.y - v2start.y) - VectorWallOrthDiffY * (v1start.x - v2start.x)) / Denominator;

	// Return the point of collision
	return {v1start.x + (PointVectorOrth * WallDiffX), v1start.y + (PointVectorOrth * WallDiffY)};
}
