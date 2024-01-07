// Copyright (C) 2014-2019 Alexandre-Xavier Labonté-Lamoureux
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
// plane.h
// Plane are used to represent polygons in the world

#include "vecmath.h"
#include "plane.h"

#include <cmath>
#include <limits>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;

// Process a plane
void Plane::Process()
{
	normal = ComputeNormal(Vertices);
	centroid = ComputeAverage(Vertices);
	SetBox();
}

void Plane::SetBox()
{
	max.x = -numeric_limits<float>::max();
	max.y = -numeric_limits<float>::max();
	max.z = -numeric_limits<float>::max();

	min.x = numeric_limits<float>::max();
	min.y = numeric_limits<float>::max();
	min.z = numeric_limits<float>::max();

	for (unsigned int i = 0; i < Vertices.size(); i++)
	{
		// x
		if (Vertices[i].x < min.x)
			min.x = Vertices[i].x;
		if (Vertices[i].x > max.x)
			max.x = Vertices[i].x;

		// y
		if (Vertices[i].y < min.y)
			min.y = Vertices[i].y;
		if (Vertices[i].y > max.y)
			max.y = Vertices[i].y;

		// z
		if (Vertices[i].z < min.z)
			min.z = Vertices[i].z;
		if (Vertices[i].z > max.z)
			max.z = Vertices[i].z;
	}
}

bool Plane::InBox2D(float x, float y, float radius) const
{
	// x
	if (x < min.x - radius)
		return false;
	if (x > max.x + radius)
		return false;

	// y
	if (y < min.y - radius)
		return false;
	if (y > max.y + radius)
		return false;

	return true;
}

bool Plane::CanWalk() const
{
	// '1' points up (floor) and '0' points to the side (wall)
	// At '0.7', it's a 45 degrees climb.
	const float WALL_ANGLE = 0.4f;

	if (Impassable && normal.z < WALL_ANGLE && normal.z > -WALL_ANGLE)
		return false;
	return true;
}

float Plane::Max() const
{
	return max.z;
}

float Plane::Min() const
{
	return min.z;
}

float Plane::Angle() const
{
	// Return an angle that can be used for sliding if this plane cannot be entered
	return atan2(normal.y, normal.x) + M_PI / 2;
}
