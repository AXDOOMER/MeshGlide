// Copyright (C) 2014-2017 Alexandre-Xavier Labonté-Lamoureux
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
// physics.h
// What affects a thing's movement in space

#ifndef _PHYSICS_H
#define _PHYSICS_H

#include "things.h"

#include <cmath>		/* round, isnan */
#include <limits>		/* numeric_limits<float>::lowest() */
using namespace std;

// Collision detection with floors
bool AdjustPlayerToFloor(Player* play, Level* lvl)
{
	float k = numeric_limits<float>::lowest();
	bool new_height = false;
	for (int i = 0; i < lvl->ptrWalls.size(); i++)
	{
		if (pointInPoly(play->PosX, play->PosY, lvl->ptrWalls[i].Vertices))
		{
			float collision_point_z = PointHeightOnPoly(play->PosX, play->PosY, play->PosZ, lvl->ptrWalls[i].Vertices);
			if (!isnan(collision_point_z) && collision_point_z > k)
			{
				if (collision_point_z <= play->PosZ + play->MaxStep)
				{
					k = collision_point_z;
					new_height = true;
				}
			}
		}
	}
	if (new_height)
	{
		if (play->PosZ <= k + GRAVITY)
		{
			play->PosZ = k;
		}
		else
		{
			play->PosZ -= GRAVITY;
		}
	}

	return new_height;
}

#endif
