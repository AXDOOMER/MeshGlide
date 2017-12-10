// Copyright (C) 2017 Alexandre-Xavier Labonté-Lamoureux
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

// Ray-casting algorithm used to find if a 2D coordinate is on a 3D polygon
bool pointInPoly(float x, float y, vector<Float3> vertices) {
	bool inside = false;
	for(int i = 0, j = vertices.size() - 1; i < vertices.size(); j = i++) {
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

Float3 crossProduct(Float3 u, Float3 v) {
	return (Float3){u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x};
}

Float3 addVectors(Float3 u, Float3 v){
	return (Float3){u.x + v.x, u.y + v.y, u.z + v.z};
}

Float3 subVectors(Float3 u, Float3 v){
	return (Float3){u.x - v.x, u.y - v.y, u.z - v.z};
}

Float3 scaleVector(float l, Float3 v) {
	return (Float3){l * v.x, l * v.y, l * v.z};
}

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

float dotProduct(Float3 u, Float3 v)
{
	return u.x * v.x + u.y * v.y + u.z * v.z;
}

// Find the intersection of a ray that aims down on a polygon
// Parameters: ray, origin of ray, polygon's normal, center of polygon
float RayIntersect(Float3 ray, Float3 origin, Float3 normal, Float3 center)
{
    // Result of dot's product of ray direction
    float denominator = dotProduct(ray, normal);

	// Direction and plane parallel, no intersection
    if (abs(denominator) < 0.00001) return numeric_limits<float>::quiet_NaN();

	Float3 difference = subVectors(center, origin);
    float t = dotProduct(difference, normal) / denominator;
    if (t < 0) return numeric_limits<float>::quiet_NaN();	// plane behind ray's origin

	Float3 contact = addVectors(origin , scaleVector(t, normalize(ray)));
    return contact.z;
}

// Find the intersection of a ray that aims down on a polygon
// Parameters: ray, origin of ray, polygon's normal, center of polygon
float RayIntersect2(Float3 ray, Float3 origin, Float3 normal, Float3 center) {
    if (dotProduct(normal, ray) == 0) {	// Use an epsilon here? (> 0.0001f)
        return numeric_limits<float>::quiet_NaN();	// No intersection, the line is parallel to the plane
    }

    // Get denominator value
    float d = dotProduct(normal, center);

    // Compute the X value for the directed line ray intersecting the plane
    float x = (d - dotProduct(normal, origin)) / dotProduct(normal, ray);

    // Output contact point, make sure the ray is normalized.
    Float3 contact = addVectors(origin , scaleVector(x, normalize(ray)));
    return contact.z;
}

// Takes a X, Y, Z position to test with polygon and the polygon's vertices
float PointHeightOnPoly(float x, float y, float z, vector<Float3> vertices) {

	Float3 origin = {x, y, z};

	float x0 = vertices[0].z;	// X
	float y0 = vertices[0].x;	// Y
	float z0 = vertices[0].y;	// Z

	float x1 = vertices[1].z;
	float y1 = vertices[1].x;
	float z1 = vertices[1].y;	// Z

	float x2 = vertices[2].z;
	float y2 = vertices[2].x;
	float z2 = vertices[2].y;	// Z

	// Vector 'u'
	Float3 u = {x1 - x0, y1 - y0, z1 - z0};

	// Vector 'v'
	Float3 v = {x2 - x0, y2 - y0, z2 - z0};

	// If the polygon is straight on the X and Y axes (no Z variation), return its height right away.
	if (z0 == z1 && z1 == z2)
		return z0;

	// Cross product to get the normal
	Float3 n = crossProduct(u, v);

	// Center of polygon
	Float3 total = {0, 0, 0};
	for (int i = 0; i < vertices.size(); i++)
	{
		total.x += vertices[i].z;
		total.y += vertices[i].x;
		total.z += vertices[i].y;
	}
	Float3 center = {total.x / vertices.size(), total.y / vertices.size(), total.z / vertices.size()};

	// Trace ray
	Float3 ray = {0, 0, -1};
	return RayIntersect2(ray, origin, n, center);
}
