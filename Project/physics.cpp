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
// physics.cpp
// What affects a thing's movement in space

#include "things.h"
#include "vecmath.h"	/* Float3 */
#include "physics.h"

#include <cmath>		/* round, isnan, fmod, nanf */
#include <limits>		/* numeric_limits */
#include <vector>
#include <utility>		/* pair */
#include <algorithm>	/* find, sort */

using namespace std;

const float WALL_ANGLE = 0.4f;	// '1' points up (floor) and '0' points to the side (wall)

// Collision detection with floors
bool AdjustPlayerToFloor(Player* play, Level* lvl)
{
	float NewHeight = numeric_limits<float>::lowest();
	bool ChangeHeight = false;
	for (unsigned int i = 0; i < lvl->planes.size(); i++)
	{
		if (pointInPoly(play->PosX(), play->PosY(), lvl->planes[i]->Vertices))
		{
			float collision_point_z = PointHeightOnPoly(play->PosX(), play->PosY(), play->PosZ(), lvl->planes[i]->normal, lvl->planes[i]->centroid);
			if (!isnan(collision_point_z) && collision_point_z > NewHeight)
			{
				if (collision_point_z <= play->PosZ() + play->MaxStep)
				{
					NewHeight = collision_point_z;
					ChangeHeight = true;
				}
			}
		}
	}
	if (ChangeHeight)
	{
		if (play->PosZ() <= NewHeight + GRAVITY)
		{
			play->pos_.z = NewHeight;
			play->AirTime = 0;
			play->Jump = false;
			play->Fly = false;
		}
		else
		{
			play->pos_.z -= GRAVITY * 0.1f * play->AirTime;
			play->AirTime++;
			play->Fly = true;	// If player is falling, but has not jumped, he must not be able to jump.
		}
	}

	return ChangeHeight;
}

void ApplyGravity(Player* play)
{
	float FloorHeight = PointHeightOnPoly(play->PosX(), play->PosY(), play->PosZ(), play->plane->normal, play->plane->centroid);
	if (!isnan(FloorHeight))
	{
		if (play->PosZ() <= FloorHeight + GRAVITY)
		{
			play->pos_.z = FloorHeight;
			play->AirTime = 0;
			play->Jump = false;
			play->Fly = false;
		}
		else
		{
			play->pos_.z -= GRAVITY * 0.1f * play->AirTime;
			play->AirTime++;
			play->Fly = true;	// If player is falling, but has not jumped, he must not be able to jump.
		}
	}
}

vector<Plane*> FindPlanesForPlayer(Player* play, Level* lvl)
{
	vector<Plane*> planes;

	for (unsigned int i = 0; i < lvl->planes.size(); i++)
	{
		if (pointInPoly(play->PosX(), play->PosY(), lvl->planes[i]->Vertices))
		{
			planes.push_back(lvl->planes[i]);
		}
	}

	return planes;
}

Plane* GetPlaneForPlayer(Player* play, Level* lvl)
{
	vector<Plane*> planes = FindPlanesForPlayer(play, lvl);
	vector<pair<Plane*, float>> HeightOnPlanes;
	float HighestUnderPlayer = numeric_limits<float>::lowest();

	// Find the height of the player on every planes
	for (unsigned int i = 0; i < planes.size(); i++)
	{
		float Height = PointHeightOnPoly(play->PosX(), play->PosY(), play->PosZ(), planes[i]->normal, planes[i]->centroid);
		HeightOnPlanes.push_back(make_pair(planes[i], Height));

		// This results in the player being put on the highest floor
		if (Height > HighestUnderPlayer && Height <= play->PosZ())
		{
			HighestUnderPlayer = Height;
		}
	}

	// Return the plane where the player is
	for (unsigned int i = 0; i < HeightOnPlanes.size(); i++)
	{
		if (HeightOnPlanes[i].second == HighestUnderPlayer)
			return HeightOnPlanes[i].first;
	}

	return nullptr;	// Need to return something
}

// Travel through planes which were intersected until it finds the one where the point is inside.
// The last parameter determines if it's going to return the last touched polygon or
// if it returns 'null' when the target point ends up in the void (is an invalid position).
Plane* TraceOnPolygons(const Float3& origin, const Float3& target, Plane* plane, bool last)
{
	vector<Plane*> tested = {plane};		// List of polys that are already checked

	for (unsigned int i = 0; i < tested[tested.size() - 1]->Neighbors.size(); i++)
	{
		Plane* p = tested[tested.size() - 1]->Neighbors[i];

		// Skip to the next adjacent plane if this one was already checked
		if (find(tested.rbegin(), tested.rend(), p) != tested.rend())
			continue;

		// Do not walk on walls. The impassable/blocking flag can be set to 0 to allow stairs.
		if (p->Impassable && p->normal.z < WALL_ANGLE && p->normal.z > -WALL_ANGLE)
			continue;

		if (pointInPoly(target.x, target.y, p->Vertices))
			return p;

		// Scan each polygon edge to see if the vector is crossing
		for (unsigned int j = 0; j < p->Vertices.size(); j++)
		{
			if (CheckVectorIntersection(origin, target, p->Vertices[j], p->Vertices[(j+1) % p->Vertices.size()]))
			{
				tested.push_back(p);
				i = -1;		// Reset outer loop
				break;
			}
		}
	}

	// Return the last touched polygon instead of the "void" if requested.
	if (last)
		return tested[tested.size() - 1];

	return nullptr;
}

// TODO: Player should not be able to step on impassable planes.
// Moves the player to a new position. Returns false if it can't.
bool MovePlayerToNewPosition(const Float3& origin, const Float3& target, Player* play)
{
	if (pointInPoly(target.x, target.y, play->plane->Vertices))
	{
		// Player is in the same polygon
		return true;
	}
	else // Player has moved to another polygon
	{
		Plane* p = TraceOnPolygons(origin, target, play->plane, false);

		// Handle this case where the player could be standing in the void
		if (p == nullptr)
			return false;	// Player may still get stuck, but we avoid a crash.

		play->plane = p;

		return true;
	}

	return false;	// When "true", allows the player to move off the edge of a polygon that has no adjacing polygon
}

// Distance smaller than length (inside or touches)
bool CompareDistanceToLength(const float DiffX, const float DiffY, const float Length)
{
	return pow(DiffX, 2) + pow(DiffY, 2) <= Length * Length;
}

float AngleOfFarthestIntersectedEdge(const Float3& origin, const Float3& target, Plane* plane)
{
	// Used to find out the farthest collision point
	float distance = 0.0f;
	unsigned int index = numeric_limits<unsigned int>::max();

	// For each edge that has an intersection, get the distance from the origin to the intersection point
	for (unsigned int i = 0; i < plane->Vertices.size(); i++)
	{
		if (CheckVectorIntersection(origin, target, plane->Vertices[i], plane->Vertices[(i+1) % plane->Vertices.size()]))
		{
			Float2 impact_point = CollisionPoint(origin, target, plane->Vertices[i], plane->Vertices[(i+1) % plane->Vertices.size()]);
			float origin_to_impact = sqrt(pow(origin.x - impact_point.x, 2) + pow(origin.y - impact_point.y, 2));
			if (origin_to_impact > distance)
			{
				distance = origin_to_impact;
				index = i;
			}
		}
	}

	if (index >= plane->Vertices.size())
	{
		// This will happen if this function is used with a vector (origin-->target) that doesn't touch the plane.
		return numeric_limits<float>::quiet_NaN();
	}

	// Compute the angle for this edge of the polygon
	float edgeAngle = atan2(plane->Vertices[index].y - plane->Vertices[(index+1) % plane->Vertices.size()].y,
							plane->Vertices[index].x - plane->Vertices[(index+1) % plane->Vertices.size()].x);

	return edgeAngle;
}

Float2 MoveOnCollision(const Float3& origin, const Float3& target, Player* play)
{
	Plane* targetPlane = TraceOnPolygons(origin, target, play->plane, true);

	for (unsigned int i = 0; i < play->plane->Vertices.size(); i++)
	{
		// Test if the player tries to get out of the plane where he is. We must make sure because it may not always be true.
		if (CheckVectorIntersection(origin, target, play->plane->Vertices[i], play->plane->Vertices[(i+1) % play->plane->Vertices.size()]))
		{
			// Get the angle that represents the direction of the player's movement
			float moveAngle = atan2(origin.y - target.y, origin.x - target.x);

			// Get the angle of the blocking wall (blocking edge of the polygon) of the target plane
			float wallAngle = AngleOfFarthestIntersectedEdge(origin, target, targetPlane);

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
	}

	// Return something that is obviously wrong if the above has failed
	return {numeric_limits<float>::quiet_NaN(), numeric_limits<float>::quiet_NaN()};
}

void Hitscan(Level* lvl, Player* play)
{
	vector<Float3> points;

	for (unsigned int i = 0; i < lvl->planes.size(); i++)
	{
		// Player's orientation angles
		float playV = play->VerticalAim;
		float playH = play->GetRadianAngle(play->Angle);

		// Get the point where the player is looking at and throw a ray
		//http://www.opengl-tutorial.org/beginners-tutorials/tutorial-6-keyboard-and-mouse/
		Float3 aim = {cos(playH) * cos(playV), sin(playH) * cos(playV), sin(playV)};
		Float3 f = RayIntersect(aim, {play->PosX(), play->PosY(), play->PosZ() + play->ViewZ}, lvl->planes[i]->normal, lvl->planes[i]->centroid);

		if (f.z != numeric_limits<float>::quiet_NaN())
		{
			vector<Float3> verts = lvl->planes[i]->Vertices;
			// NOTE: creating a new vector for each coordinates is slower than having a 'pointInPoly' function for each set of two coordinates
			vector<float> vx = createVectorOfX(verts);
			vector<float> vy = createVectorOfY(verts);
			vector<float> vz = createVectorOfZ(verts);

			bool XY = pointInPoly(f.x, f.y, vx, vy);
			bool YZ = pointInPoly(f.y, f.z, vy, vz);
			bool ZX = pointInPoly(f.z, f.x, vz, vx);

			if ((XY && YZ) || (YZ && ZX) || (ZX && XY) || (XY && allEqual(vz)) || (YZ && allEqual(vx)) || (ZX && allEqual(vy)))
			{
				float front = atan2(play->PosY() - f.y, play->PosX() - f.x) - playH;

				if (front > M_PI_4 || front < -M_PI_4)
					points.push_back(f);
			}
		}
	}

	sort(points.begin(), points.end(), [play](const Float3 a, const Float3 b)
	{
		return pow(play->PosX() - a.x, 2) + pow(play->PosY() - a.y, 2) < pow(play->PosX() - b.x, 2) + pow(play->PosY() - b.y, 2);
	});

	if (points.size() > 0)
	{
		lvl->things.push_back(new Puff(points[0].x, points[0].y, points[0].z));
	}
}
