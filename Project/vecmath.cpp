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
bool pointInPoly(const float x, const float y, const vector<Float3>& vertices)
{
	bool inside = false;
	for(unsigned int i = 0, j = vertices.size() - 1; i < vertices.size(); j = i++) {
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

// Find the intersection of a ray that aims down on a polygon
// Parameters: ray, origin of ray, polygon's normal, center of polygon
float RayIntersect(const Float3& ray, const Float3& origin, const Float3& normal, const Float3& center)
{
	if (dotProduct(normal, ray) == 0) // Use an epsilon here? (< 0.0001f)
		return numeric_limits<float>::quiet_NaN();	// No intersection, the line is parallel to the plane

	// Get denominator value
	float d = dotProduct(normal, center);

	// Compute the X value for the directed line ray intersecting the plane
	float x = (d - dotProduct(normal, origin)) / dotProduct(normal, ray);

	// Return the height of the contact point
	return addVectors(origin , scaleVector(x, ray)).z;
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

// Takes a X, Y, Z position to test with polygon and the polygon's vertices
float PointHeightOnPoly(const float x, const float y, const float z, const vector<Float3>& vertices, const Float3& normal)
{
	// If the polygon is straight on the X and Y axes (no Z variation), return its height right away.
	if (vertices[0].z ==  vertices[1].z && vertices[1].z == vertices[2].z)
		return vertices[0].z;

	// Center of polygon
	Float3 total = {0, 0, 0};
	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		total.x += vertices[i].x;
		total.y += vertices[i].y;
		total.z += vertices[i].z;
	}
	Float3 center = {total.x / vertices.size(), total.y / vertices.size(), total.z / vertices.size()};

	// Trace a ray that aims down
	return RayIntersect({0, 0, -1}, {x, y, z}, normal, center);
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
