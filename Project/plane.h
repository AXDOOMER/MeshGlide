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

#ifndef PLANE_H
#define PLANE_H

#include "texture.h"
#include "vecmath.h"

#include <string>
#include <vector>

using namespace std;

struct Edge
{
	Float3 a;	// First
	Float3 b;	// Second
	short sides;
	vector<Float3> Vertices;
	float angle;
};

class Plane
{
public:
	string Texture;
	bool Impassable = true;
	bool TwoSided = false;
	vector<Float3> Vertices;
	vector<Float2> UVs;
	float Xscale = 0;
	float Yscale = 0;
	float Xoff = 0;
	float Yoff = 0;
	float Light = 1;	// Must be between 0 (dark) and 1 (full bright)

	Float3 normal;
	Float3 centroid;
	vector<Plane*> Neighbors;	// List of adjacent planes
	vector<Edge> Edges;	// List of adjacent planes

private:
	Float3 max;		// Maximal coordinates
	Float3 min;		// Minimal coordinates

public:
	float Max() const;
	float Min() const;

	float Angle() const;

	void Process();		// Find centroid, find normal...
	unsigned int CommonVertices(Plane* plane);

	void SetBox();
	bool InBox2D(float x, float y, float radius) const;

	bool CanWalk() const;
};

#endif /* PLANE_H */
