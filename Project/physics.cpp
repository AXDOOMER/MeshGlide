// Copyright (C) 2014-2018 Alexandre-Xavier Labont�-Lamoureux
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
// physics.cpp
// What affects a thing's movement in space (collision detection and response)

#include "player.h"
#include "plane.h"
#include "vecmath.h"	/* Float3 */
#include "physics.h"
#include "line.h"

#include <cmath>		/* round, isnan, fmod, nanf */
#include <limits>		/* numeric_limits */
#include <vector>
#include <utility>		/* pair */
#include <algorithm>	/* find, sort */
#include <stdexcept>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;

// Cap a value between a min and a max
float CapHeight(float point, float min, float max)
{
	if (point > max)
		return max;
	if (point < min)
		return min;
	return point;
}

// Test if the player is inside the polygon or touching one of its edges
bool TouchesPlane(const Player* play, const Plane* p)
{
	// Is the player inside the polygon?
	if (pointInPoly(play->PosX(), play->PosY(), p->Vertices))
		return true;

	// Is the player touching one of the polygon's edges?
	for (unsigned int i = 0, j = p->Vertices.size() - 1; i < p->Vertices.size(); j = i++)
		if (lineCircle(p->Vertices[i].x, p->Vertices[i].y, p->Vertices[j].x, p->Vertices[j].y,play->PosX(), play->PosY(), play->Radius()))
			return true;

	return false;
}

// Get every plane toucehd by a player
vector<Plane*> TouchedPlanes(const Player* play, const Level* lvl)
{
	vector<Plane*> touched;

	// List of potential planes that can be touched. Others were discarded.
	vector<Plane*> potential = lvl->getPlanesForBox(play->pos_.x, play->pos_.y, play->Radius());

	// Make a the list of planes that were toucehd
	for (unsigned int i = 0; i < potential.size(); i++)
		if (TouchesPlane(play, potential[i]))		// Player touches the polygon
			touched.push_back(potential[i]);

	return touched;
}

// Collision detection with floors
bool AdjustPlayerToFloor(Player* play, Level* lvl)
{
	float NewHeight = numeric_limits<float>::lowest();
	bool ChangeHeight = false;	// Note: Making this true will allow the player to fall in the void

	vector<Plane*> touched = TouchedPlanes(play, lvl);

	for (unsigned int i = 0; i < touched.size(); i++)
	{
		float FloorHeight = PointHeightOnPoly(play->PosX(), play->PosY(), play->PosZ(), touched[i]->normal, touched[i]->centroid);

		if (!isnan(FloorHeight))
		{
			FloorHeight = CapHeight(FloorHeight, touched[i]->Min(), touched[i]->Max());	// Used to better handle slopes

			if (FloorHeight > NewHeight)
			{
				if (FloorHeight <= play->PosZ() + play->MaxStep)
				{
					NewHeight = FloorHeight;
					ChangeHeight = true;
				}
			}
		}
	}

	// Applies the gravity
	if (ChangeHeight)
	{
		if (play->PosZ() <= NewHeight + GRAVITY)
		{
			play->pos_.z = NewHeight;
			play->AirTime = 0;
		}
		else
		{
			play->pos_.z -= GRAVITY * 0.1f * play->AirTime;
			play->AirTime++;
		}
	}

	return ChangeHeight;
}

// Must be used for wall segments (planes that block the player's movement) above or below the player
bool BlocksPlayer(const Player* play, float min, float max)
{
	if (play->PosZ() + play->MaxStep >= max || play->PosZ() + play->Height() <= min)
		return false;

	return true;
}

// Returns true of the player touches obstructing walls
bool PlayerTouchesWalls(const Player* play, const vector<Plane*>& touched)
{
	for (unsigned int i = 0; i < touched.size(); i++)
		if (!touched[i]->CanWalk() && BlocksPlayer(play, touched[i]->Min(), touched[i]->Max()))
			return true;

	return false;
}

// Returns a list of obstructing walls
vector<Plane*> PlayerTouchedWallsList(const Player* play, const vector<Plane*>& touched)
{
	vector<Plane*> obstructors;

	for (unsigned int i = 0; i < touched.size(); i++)
		if (!touched[i]->CanWalk() && BlocksPlayer(play, touched[i]->Min(), touched[i]->Max()))
			obstructors.push_back(touched[i]);

	return obstructors;
}

Float2 MoveAlongAngle(const Float3& origin, const Float3& target, const float theAngle)
{
	// Get the angle that represents the direction of the player's movement
	float moveAngle = atan2(origin.y - target.y, origin.x - target.x);

	// Get the angle of the blocking wall
	float wallAngle = theAngle;

	// Compute the difference between both angles
	float newAngle = wallAngle - moveAngle;

	// Make the angle positive. This is to constrain the possible angle values between 0 and PI * 2.
	if (newAngle < 0)
		newAngle += M_PI * 2;

	// Check at which direction the player is going against the wall
	if (newAngle < M_PI / 2 || newAngle > 3 * M_PI / 2)
		wallAngle += M_PI;

	// Compute the movement speed at that angle
	float moveSpeed = sqrt(pow(origin.x - target.x, 2) + pow(origin.y - target.y, 2));
	moveSpeed = moveSpeed * abs(cos(newAngle));

	// Apply that new computer position so that the player appears to be moving along that wall
	Float3 Return = origin;
	Return.x += moveSpeed * cos(wallAngle);
	Return.y += moveSpeed * sin(wallAngle);

	return {Return.x, Return.y};
}

// The position is invalid if the player touches a wall
bool NewPositionIsValid(const Player* play, const Level* lvl)
{
	return !PlayerTouchesWalls(play, TouchedPlanes(play, lvl));
}

// Distance smaller than length (inside or touches)
bool CompareDistanceToLength(const float DiffX, const float DiffY, const float Length)
{
	return pow(DiffX, 2) + pow(DiffY, 2) <= Length * Length;
}

Float2 SlideOnCollision(const Float3& origin, const Float3& target, const Plane* obstruction)
{
	// Get the angle that represents the direction of the player's movement
	float moveAngle = atan2(origin.y - target.y, origin.x - target.x);

	// Get the angle of the blocking wall
	float wallAngle = obstruction->Angle();

	// Compute the difference between both angles
	float newAngle = wallAngle - moveAngle;

	// Make the angle positive. This is to constrain the possible angle values between 0 and PI * 2.
	if (newAngle < 0)
		newAngle += M_PI * 2;

	// Check at which direction the player is going against the wall
	if (newAngle < M_PI / 2 || newAngle > 3 * M_PI / 2)
		wallAngle += M_PI;

	// Compute the movement speed at that angle
	float moveSpeed = sqrt(pow(origin.x - target.x, 2) + pow(origin.y - target.y, 2));
	moveSpeed = moveSpeed * abs(cos(newAngle));

	// Apply that new computer position so that the player appears to be moving along that wall
	Float3 Return = origin;
	Return.x += moveSpeed * cos(wallAngle);
	Return.y += moveSpeed * sin(wallAngle);

	return {Return.x, Return.y};
}

// Push player out of walls if it's stuck
Float2 MoveOnCollision(const Float3& origin, const Float3& target, const Player* play, const Level* lvl)
{
	// Check if the player is stuck
	if (PlayerTouchesWalls(play, TouchedPlanes(play, lvl)))
	{
		// Get the list of touched walls
		const vector<Plane*> touched = PlayerTouchedWallsList(play, TouchedPlanes(play, lvl));

		Player* dummy = new Player();	// Used for simulations
		dummy->pos_.z = play->pos_.z;	// The "dummy" must be at the same height as the player

		// Try to move slide the player at least three times
		for (unsigned int i = 0; i < touched.size() && i < 3; i++)
		{
			Float2 myNewPos = SlideOnCollision(origin, target, touched[i]);

			dummy->pos_.x = myNewPos.x;
			dummy->pos_.y = myNewPos.y;

			if (PlayerTouchesWalls(dummy, TouchedPlanes(dummy, lvl)))
			{
				// Didn't work, try again...
				continue;
			}
			else
			{
				// It worked. Return this new valid position.
				return myNewPos;
			}

		}

		// It never worked. Return the original position.
		return {origin.x, origin.y};
	}
	else
	{
		// Nothing is blocking the player from moving to its target position
		return {target.x, target.y};
	}
}

// NOTE: Function is huge and ugly. Could be improved.
void Hitscan(Level* lvl, Player* play, const vector<Player*>& players)
{
	// 0.57735 is the biggest value for x,y,z that you can get all at once for a normal in a sphere
	// This value would be 0.70711 in a circle
	const float THRESHOLD = 0.5f;

	Float3 wallHitPoint;
	Plane* planeHitPoint = nullptr;
	float wallDist = numeric_limits<float>::max();

	// Check for polygons that are hit.
	for (unsigned int i = 0; i < lvl->planes.size(); i++)
	{
		// Get the point where the player is looking at and throw a ray
		//http://www.opengl-tutorial.org/beginners-tutorials/tutorial-6-keyboard-and-mouse/
		Float3 aim = {play->AimX(), play->AimY(), play->AimZ()};
		Float3 f = RayIntersect(aim, {play->PosX(), play->PosY(), play->CamZ()}, lvl->planes[i]->normal, lvl->planes[i]->centroid);

		// Check if point is valid
		if (f.z != numeric_limits<float>::quiet_NaN())
		{
			vector<Float3> verts = lvl->planes[i]->Vertices;

			// Check if the point is inside the polygon (because a plane is infinite)
			bool test = false;
			if (lvl->planes[i]->normal.z >= THRESHOLD || lvl->planes[i]->normal.z <= -THRESHOLD)
				test = pointInPoly(f.x, f.y, verts, 0, 1);	// xOy
			else if (lvl->planes[i]->normal.x >= THRESHOLD || lvl->planes[i]->normal.x <= -THRESHOLD)
				test = pointInPoly(f.y, f.z, verts, 1, 2);	// yOz
			else if (lvl->planes[i]->normal.y >= THRESHOLD || lvl->planes[i]->normal.y <= -THRESHOLD)
				test = pointInPoly(f.z, f.x, verts, 2, 0);	// zOx
			else
				throw runtime_error("Caught a polygon with the following normal:\n" +
					to_string(lvl->planes[i]->normal.x) + ", " + to_string(lvl->planes[i]->normal.y) + ", " + to_string(lvl->planes[i]->normal.z));

			if (test)
			{
				float front = atan2(play->PosY() - f.y, play->PosX() - f.x) - play->GetRadianAngle(play->Angle);

				if (front > M_PI_4 || front < -M_PI_4)
				{
					// How far is it?
					float dist = sqrt(pow(f.x - play->PosX(), 2) + pow(f.y - play->PosY(), 2) + pow(f.z - play->CamZ(), 2));

					if (planeHitPoint)
					{
						// Check if this hit is closer
						if (dist < wallDist)
						{
							wallHitPoint = f;	// Set hit position
							planeHitPoint = lvl->planes[i];	// Set wall that was hit
						}
					}
					else
					{
						wallHitPoint = f;	// Set hit position
						planeHitPoint = lvl->planes[i];	// Set wall that was hit
						wallDist = dist;	// Set distance of hit
					}
				}
			}
		}
	}

	Player* hit = nullptr;
	float hitDist = 0;
	Float3 target;

	// Check for players that are hit.
	for (unsigned int i = 0; i < players.size(); i++)
	{
		if (play != players[i])
		{
			// Compute a target point using the distance at which the player should be in order to be hit
			float distance = sqrt(pow(play->PosX() - players[i]->PosX(), 2) + pow(play->PosY() - players[i]->PosY(), 2));
			float targetX = play->PosX() + distance * cos(play->GetRadianAngle(play->Angle));
			float targetY = play->PosY() + distance * sin(play->GetRadianAngle(play->Angle));

			// Is the target point within the player's radius?
			float distanceHit = sqrt(pow(targetX - players[i]->PosX(), 2) + pow(targetY - players[i]->PosY(), 2));

			if (distanceHit < players[i]->Radius())
			{
				float targetZ = play->CamZ() + distance * tan(play->VerticalAim);

				if (targetZ >= players[i]->PosZ() && targetZ <= players[i]->PosZ() + players[i]->Height())
				{
					// If there's no previous hit, set it right away.
					// Else, check if the new hit is closer than the previous one.
					if (hit == nullptr || distance < sqrt(pow(play->PosX() - hit->PosX(), 2) + pow(play->PosY() - hit->PosY(), 2)))
					{
						// Found a closer hit
						hit = players[i];
						hitDist = sqrt(pow(play->PosX() - hit->PosX(), 2) + pow(play->PosY() - hit->PosY(), 2));
						target = {targetX, targetY, targetZ};
					}
				}
			}
		}
	}

	if (planeHitPoint && (hit == nullptr || wallDist < hitDist))
	{
		Float3 dir = {wallHitPoint.x - play->PosX(), wallHitPoint.y - play->PosY(), wallHitPoint.z - play->CamZ()};
		dir = subVectors(dir, scaleVector(0.1f, normalize(dir)));	// The puff must not touch the wall
		//dir = addVectors(dir, scaleVector(0.1f, planeHitPoint->normal));	// TODO: Use this later when every normal will point inside the level
		lvl->things.push_back(new Puff(play->PosX() + dir.x, play->PosY() + dir.y, play->CamZ() + dir.z));
	}
	else if (hit)
	{
		// Spawn blood on the targeted player. Move the blood a bit forward so it always appears in front of the sprite when the target is still.
		lvl->things.push_back(new Blood(
			target.x + 0.1f * cos(play->GetRadianAngle(play->Angle) + M_PI),
			target.y + 0.1f * sin(play->GetRadianAngle(play->Angle) + M_PI),
			target.z,
			hit->PosZ()));
	}
}

// Push something outside of a point to the specified distance (p_rad)
Float3 PushTargetOutOfPoint(const Float3& target, const Float3& point, const float p_rad)
{
	Float3 newpoint = target;
	float distance = sqrt(pow(target.x - point.x, 2) + pow(target.y - point.y, 2));
	float radii = p_rad;

	if (distance < radii)
	{
		float angle = atan2(target.y - point.y, target.x - point.x);

		Float3 pos = {target.x, target.y, 0};

		pos.x += (radii - distance) * cos(angle);
		pos.y += (radii - distance) * sin(angle);

		newpoint.x = pos.x;
		newpoint.y = pos.y;
	}

	return newpoint;
}

Float3 PlayerToPlayerCollisionReact(const Player* moved, const Player* other)
{
	const float EPSILON = 1.05f;	// To avoid still touching the player once pushed out of it, push it away by 105%.

	float distance = sqrt(pow(moved->PosX() - other->PosX(), 2) + pow(moved->PosY() - other->PosY(), 2));
	float radii = moved->Radius() + other->Radius();

	if (distance < radii)
	{
		float angle = atan2(moved->PosY() - other->PosY(), moved->PosX() - other->PosX());

		Float3 pos = moved->pos_;

		pos.x += (radii - distance) * cos(angle) * EPSILON;
		pos.y += (radii - distance) * sin(angle) * EPSILON;

		return pos;	// New position
	}

	return moved->pos_;	// Original position
}

bool PlayerToPlayerCollision(const Player* moved, const Player* other)
{
	float distance = sqrt(pow(moved->PosX() - other->PosX(), 2) + pow(moved->PosY() - other->PosY(), 2));
	float radii = moved->Radius() + other->Radius();

	if (distance < radii)
		return true;

	return false;
}

bool PlayerToPlayersCollision(const Player* source, const vector<Player*>& players)
{
	for (unsigned int i = 0; i < players.size(); i++)
	{
		if (source != players[i])
		{
			float distance = sqrt(pow(source->PosX() - players[i]->PosX(), 2) + pow(source->PosY() - players[i]->PosY(), 2));
			float radii = source->Radius() + players[i]->Radius();

			if (distance < radii)
				return true;
		}
	}

	return false;
}

// TODO: Remove if unused
bool PlayerHeightCheck(const Player* moved, const Player* other)
{
	if (abs(moved->PosZ() - other->PosZ()) <= moved->Height())
		return true;
	return false;
}

vector<Player*> GetPlayersTouched(const Player* source, const vector<Player*>& players)
{
	vector<Player*> list;

	for (unsigned int i = 0; i < players.size(); i++)
	{
		if (source != players[i])
		{
			float distance = sqrt(pow(source->PosX() - players[i]->PosX(), 2) + pow(source->PosY() - players[i]->PosY(), 2));
			float radii = source->Radius() + players[i]->Radius();

			if (distance < radii)
				list.push_back(players[i]);
		}
	}

	return list;
}
