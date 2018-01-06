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

#include "things.h"	/* Player */
#include "vecmath.h"	/* Float3 */
#include "level.h"	/* Level */
using namespace std;

// Collision detection with floors
bool AdjustPlayerToFloor(Player* play, Level* lvl);

// Distance smaller than length (inside or touches)
bool CompareDistanceToLength(float DiffX, float DiffY, float Length);

// Returns true if the vector hits any walls. The vector has a circular endpoint.
bool HitsWall(Float3 origin, Float3 target, float RadiusToUse, Level* lvl);

#endif	// _PHYSICS_H
