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
// vecmath.h
// Custom library for vector math, collision with planes, etc. that uses floats.

#ifndef VECMATH_H
#define VECMATH_H

#include <vector>

using namespace std;

// Can be used to define a data that can be expressed as a 3D coordinate, such as a vertex or a vector.
struct Float3
{
	float x;
	float y;
	float z;
};

struct Float2
{
	float x;
	float y;
};

bool operator==(const Float3& lhs, const Float3& rhs);

// Ray-casting algorithm used to find if a 2D coordinate is on a 3D polygon
bool pointInPoly(const float x, const float y, const vector<Float3>& vertices);
// References:
// http://alienryderflex.com/polygon/
// https://github.com/substack/point-in-polygon/blob/master/index.js
// http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html

Float3 crossProduct(const Float3& u, const Float3& v);

Float3 addVectors(const Float3& u, const Float3& v);

Float3 subVectors(const Float3& u, const Float3& v);

Float3 scaleVector(const float l, const Float3& v);

Float3 normalize(Float3 v);

float dotProduct(const Float3& u, const Float3& v);
// References:
// https://rosettacode.org/wiki/Find_the_intersection_of_a_line_with_a_plane#C
// https://github.com/id-Software/Quake-III-Arena/blob/master/code/game/q_math.c

// Find the intersection of a ray that aims down on a polygon
float RayIntersect(const Float3& ray, const Float3& origin, const Float3& normal, const Float3& center);
// References:
// https://samsymons.com/blog/math-notes-ray-plane-intersection/
// http://cmichel.io/howto-raytracer-ray-plane-intersection-theory/
// https://rosettacode.org/wiki/Find_the_intersection_of_a_line_with_a_plane
// https://stackoverflow.com/questions/23975555/how-to-do-ray-plane-intersection
// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-plane-and-ray-disk-intersection

Float3 ComputeNormal(const vector<Float3>& vertices);
Float3 ComputeAverage(const vector<Float3>& vertices);

// Get height on a polygon
float PointHeightOnPoly(const float x, const float y, const float z, const Float3& normal, const Float3& centroid);

// Returns true if two vectors intersect
bool CheckVectorIntersection(const Float3& v1start, const Float3& v1end, const Float3& v2start, const Float3& v2end);

#endif	// VECMATH_H
