// Copyright (C) 2014-2018 Alexandre-Xavier Labonté-Lamoureux
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
#include "vecmath.h"	// Custom library for vector math, collision with planes, etc.

#include <cmath>		/* round, isnan */
#include <limits>		/* numeric_limits<float>::lowest() */
using namespace std;

// Collision detection with floors
bool AdjustPlayerToFloor(Player* play, Level* lvl)
{
	float NewHeight = numeric_limits<float>::lowest();
	bool ChangeHeight = false;
	for (int i = 0; i < lvl->planes.size(); i++)
	{
		if (pointInPoly(play->PosX, play->PosY, lvl->planes[i]->Vertices))
		{
			float collision_point_z = PointHeightOnPoly(play->PosX, play->PosY, play->PosZ, lvl->planes[i]->Vertices);
			if (!isnan(collision_point_z) && collision_point_z > NewHeight)
			{
				if (collision_point_z <= play->PosZ + play->MaxStep)
				{
					NewHeight = collision_point_z;
					ChangeHeight = true;
				}
			}
		}
	}
	if (ChangeHeight)
	{
		if (play->PosZ <= NewHeight + GRAVITY)
		{
			play->PosZ = NewHeight;
			play->AirTime = 0;
			play->Jump = false;
			play->Fly = false;
		}
		else
		{
			play->PosZ -= GRAVITY * 0.1f * play->AirTime;
			play->AirTime++;
			play->Fly = true;	// If player is falling, but has not jumped, he must not be able to jump.
		}
	}

	return ChangeHeight;
}

// Distance smaller than length (inside or touches)
bool CompareDistanceToLength(float DiffX, float DiffY, float Length)
{
	return pow(DiffX, 2) + pow(DiffY, 2) <= Length * Length;
}

// Returns true if the vector hits any walls. The vector has a circular endpoint.
bool HitsWall(Float3 origin, Float3 target, float RadiusToUse, Level* lvl)
{
	for (int i = 0; i < lvl->planes.size(); i++)
	{
		// Test vertical planes which are walls
		if (lvl->planes[i]->WallInfo != nullptr)
		{
			// Wall above or bellow player.
			if (lvl->planes[i]->WallInfo->HighZ <= origin.z + lvl->play->MaxStep || lvl->planes[i]->WallInfo->LowZ >= origin.z + 3.0f)
			{
				// Can't be any collision.
				continue;
			}

			// Create new variables for readability
			Float3 First = lvl->planes[i]->WallInfo->Vertex1;
			Float3 Second = lvl->planes[i]->WallInfo->Vertex2;

			// Get the orthogonal vector, so invert the use of 'sin' and 'cos' here.
			float OrthVectorStartX = origin.x + sin(lvl->planes[i]->WallInfo->Angle) * RadiusToUse;
			float OrthVectorStartY = origin.y + cos(lvl->planes[i]->WallInfo->Angle) * RadiusToUse;
			float OrthVectorEndX = origin.x - sin(lvl->planes[i]->WallInfo->Angle) * RadiusToUse;
			float OrthVectorEndY = origin.y - cos(lvl->planes[i]->WallInfo->Angle) * RadiusToUse;

			// Cramer's rule, inspiration taken from here: https://stackoverflow.com/a/1968345
			float WallDiffX = Second.x - First.x;    // Vector's X from (0,0)
			float WallDiffY = Second.y - First.y;    // Vector's Y from (0,0)
			float VectorWallOrthDiffX = OrthVectorEndX - OrthVectorStartX;
			float VectorWallOrthDiffY = OrthVectorEndY - OrthVectorStartY;

			float Denominator = -VectorWallOrthDiffX * WallDiffY + WallDiffX * VectorWallOrthDiffY;
			float PointWall = (-WallDiffY * (First.x - OrthVectorStartX) + WallDiffX * (First.y - OrthVectorStartY)) / Denominator;
			float PointVectorOrth = (VectorWallOrthDiffX * (First.y - OrthVectorStartY) - VectorWallOrthDiffY * (First.x - OrthVectorStartX)) / Denominator;

			// Check if a collision is detected (Not checking if touching an endpoint)
			if (PointWall >= 0 && PointWall <= 1 && PointVectorOrth >= 0 && PointVectorOrth <= 1)
			{
				// Collision detected
//				cout << "Collision at " << First.x + (PointVectorOrth * WallDiffX) << ", " << First.y + (PointVectorOrth * WallDiffY) << endl;	// DEBUG
				return true;
			}

			// TODO: Check for a collision with both endpoints of the wall
		}
	}

	return false;
}

#endif	// _PHYSICS_H
