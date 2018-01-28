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

#ifndef PHYSICS_H
#define PHYSICS_H

#include "things.h"	/* Player */
#include "vecmath.h"	/* Float3 */
#include "level.h"	/* Level */

// Collision detection with floors
bool AdjustPlayerToFloor(Player* play, Level* lvl);

void ApplyGravity(Player* play);

// Distance smaller than length (inside or touches)
bool CompareDistanceToLength(const float DiffX, const float DiffY, const float Length);

// Returns true if the vector hits any walls. The vector has a circular endpoint.
bool HitsWall(const Float3& point, const float RadiusToUse, Level* lvl);

// Moves the player to a new position. Returns false if it can't.
bool MovePlayerToNewPosition(const Float3& origin, const Float3& target, Player* play);

// Get the plane where the player is standing
Plane* GetPlaneForPlayer(Player* play, Level* lvl);

#endif	// PHYSICS_H
