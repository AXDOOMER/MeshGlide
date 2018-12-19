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
// What affects a thing's movement in space (collision detection and response)

#include "things.h"
#include "vecmath.h"	/* Float3 */
#include "physics.h"
#include "line.h"

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
		if (pointInPolyXY(play->PosX(), play->PosY(), lvl->planes[i]->Vertices))
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
		}
		else
		{
			play->pos_.z -= GRAVITY * 0.1f * play->AirTime;
			play->AirTime++;
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
		}
		else
		{
			play->pos_.z -= GRAVITY * 0.1f * play->AirTime;
			play->AirTime++;
		}
	}
}

vector<Plane*> FindPlanesForPlayer(Player* play, Level* lvl)
{
	vector<Plane*> planes;

	for (unsigned int i = 0; i < lvl->planes.size(); i++)
	{
		if (pointInPolyXY(play->PosX(), play->PosY(), lvl->planes[i]->Vertices))
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

		// Do not walk on walls
		if (!p->CanWalk())
			continue;

		if (pointInPolyXY(target.x, target.y, p->Vertices))
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

// Test if the player is inside the polygon or touching one of its edges
bool TouchPlane(const Player* play, const Plane* p)
{
	// Is the player inside the polygon?
	if (pointInPolyXY(play->PosX(), play->PosY(), p->Vertices))
		return true;

	// Is the player touching an edge of the polygon?
	for (unsigned int i = 0; i < p->Edges.size(); i++)
		if (lineCircle(p->Edges[i].a.x, p->Edges[i].a.y, p->Edges[i].b.x, p->Edges[i].b.y,play->PosX(), play->PosY(), play->Radius()))
			return true;

	return false;
}

// Test if the player is touching a plane and find the neighbors to that plane
void TouchingPlanes(const Player* play, Plane* p, vector<Plane*>& Tested)
{
	// Player touches the polygon
	if (TouchPlane(play, p))
	{
		if (p->CanWalk())
			Tested.push_back(p);
		else
			return;

		// Try its neighbors
		for (unsigned int i = 0; i < p->Neighbors.size(); i++)
		{
			// Only test if not previously tested
			if (find(Tested.begin(), Tested.end(), p->Neighbors[i]) == Tested.end())
				TouchingPlanes(play, p->Neighbors[i], Tested);
		}
	}
}

Float2 MoveAlongAngle(const Float3& origin, const Float3& target, const float theAngle)
{
	// Get the angle that represents the direction of the player's movement
	float moveAngle = atan2(origin.y - target.y, origin.x - target.x);

	// Get the angle of the blocking wall (blocking edge of the polygon) of the target plane
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

// Check if the player (including his radius) is touching edges that may block him
bool RadiusClearOfEdges(const Float3& target, const Player* play)
{
	// Check the planes that are touched
	vector<Plane*> pTouched;
	TouchingPlanes(play, play->plane, pTouched);

	// Check for the edges that are touched
	vector<Edge> edges;
	for (unsigned int j = 0; j < pTouched.size(); j++)
	{
		for (unsigned int i = 0; i < pTouched[j]->Edges.size(); i++)
		{
			bool touch = lineCircle(pTouched[j]->Edges[i].a.x, pTouched[j]->Edges[i].a.y, 
				pTouched[j]->Edges[i].b.x, pTouched[j]->Edges[i].b.y, target.x, target.y, play->Radius());

			if (touch)
			{
				edges.push_back(pTouched[j]->Edges[i]);
			}
		}
	}

	for (int i = edges.size() - 1; i >= 0; i--)
	{
		// If the is not blocking, remove it.
		if (edges[i].sides > 0)
		{
			edges.erase(edges.begin() + i);
		}
	}

	// The player scrossed the edge of the polygon
	if (edges.size() > 0)
	{
		// Intersection. Can't pass over there.
		return true;
	}

	// No intersection
	return false;
}

// TODO: Player should not be able to step on impassable planes.
// Moves the player to a new position. Returns false if it failed.
bool MovePlayerToNewPosition(const Float3& origin, Float3 target, Player* play)
{
	if (RadiusClearOfEdges(target, play))
	{
		return false;
	}

	if (pointInPolyXY(target.x, target.y, play->plane->Vertices))
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

// Push player out of point (extremity of a line)
// TODO: Deduplicate code that's also found in "RadiusClearOfEdges()"
// TODO: Reduce size. Function is big and complicated.
Float2 MoveOnCollision(const Float3& origin, const Float3& target, const Player* play)
{
	// Get planes that are touched by the player
	vector<Plane*> pTouched;
	TouchingPlanes(play, play->plane, pTouched);

	// Check the edges of polygons that are touched
	vector<Edge> edges;
	for (unsigned int j = 0; j < pTouched.size(); j++)
	{
		for (unsigned int i = 0; i < pTouched[j]->Edges.size(); i++)
		{
			bool touch = lineCircle(pTouched[j]->Edges[i].a.x, pTouched[j]->Edges[i].a.y, 
				pTouched[j]->Edges[i].b.x, pTouched[j]->Edges[i].b.y, target.x, target.y, play->Radius());

			if (touch)
			{
				edges.push_back(pTouched[j]->Edges[i]);
			}
		}
	}

	// Erase edges that are not at least one-sided
	for (int i = edges.size() - 1; i >= 0; i--)
	{
		if (edges[i].sides > 0)
		{
			edges.erase(edges.begin() + i);
		}
	}

	// TODO: For list of possible edges, get the distance from them
	// (perpendicular and from the start point and end point)
	// Then find the closest edge and slide against it.

	// The player crossed the edge of the polygon
	if (edges.size() > 0)
	{
		vector<Edge> touching;
		vector<Float2> points;
		vector<Float2> pointsintersection;
		Float3 newpoint = origin;

		for (unsigned int i = 0; i < edges.size(); i++)
		{
			// Check for extremities
			if (CompareDistanceToLength(edges[i].a.x - target.x, edges[i].a.y - target.y, play->Radius()))
			{
				points.push_back({edges[i].a.x, edges[i].a.y});
			}

			if (CompareDistanceToLength(edges[i].b.x - target.x, edges[i].b.y - target.y, play->Radius()))
			{
				points.push_back({edges[i].b.x, edges[i].b.y});
			}

			// Check along the line
			float angle = (float)atan2(edges[i].a.y - edges[i].b.y, edges[i].a.x - edges[i].b.x);

			// Get the orthogonal vector by adding 90 degrees to the angle.
			float OrthPlayerStartX = target.x + (float) cos(angle + M_PI_2) * play->Radius();
			float OrthPlayerStartY = target.y + (float) sin(angle + M_PI_2) * play->Radius();
			float OrthPlayerEndX = target.x - (float) cos(angle + M_PI_2) * play->Radius();
			float OrthPlayerEndY = target.y - (float) sin(angle + M_PI_2) * play->Radius();

			if (CheckVectorIntersection(edges[i].a, edges[i].b, {OrthPlayerStartX, OrthPlayerStartY, 0}, {OrthPlayerEndX, OrthPlayerEndY, 0}))
			{
				// Intersection
				pointsintersection.push_back(CollisionPoint(edges[i].a, edges[i].b,
					{OrthPlayerStartX, OrthPlayerStartY, 0}, {OrthPlayerEndX, OrthPlayerEndY, 0}));
				touching.push_back(edges[i]);
			}
		}

		bool isWall = false;
		float distance = numeric_limits<float>::max();

		const float EPSILON = 0.05f;

		for (unsigned int k = 0; k < points.size(); k++)
		{
			if (distance > sqrt( pow(points[k].x, 2) + pow(points[k].y, 2) ))
			{
				newpoint = PushTargetOutOfPoint(target, {points[k].x, points[k].y, 0}, play->Radius() + EPSILON);
			}
		}

		Edge closest;

		for (unsigned int k = 0; k < touching.size(); k++)
		{
			if (distance > sqrt( pow(pointsintersection[k].x, 2) + pow(pointsintersection[k].y, 2) ))
			{
				newpoint = PushTargetOutOfPoint(target, {pointsintersection[k].x, pointsintersection[k].y, 0}, play->Radius());
				isWall = true;
				closest = touching[k];
			}
		}

		if (isWall)
		{
			float theAngle = (float)atan2(closest.a.y - closest.b.y, closest.a.x - closest.b.x);
			return MoveAlongAngle(origin, target, theAngle);
		}
		else
		{
			// Intersection. Can't pass over there.
			return {newpoint.x, newpoint.y};
		}
	}

	// No intersection
	return {origin.x, origin.y};
}

void Hitscan(Level* lvl, Player* play)
{
	vector<pair<Float3, Plane*>> points;

	for (unsigned int i = 0; i < lvl->planes.size(); i++)
	{
		// Get the point where the player is looking at and throw a ray
		//http://www.opengl-tutorial.org/beginners-tutorials/tutorial-6-keyboard-and-mouse/
		Float3 aim = {play->AimX(), play->AimY(), play->AimZ()};
		Float3 f = RayIntersect(aim, {play->PosX(), play->PosY(), play->PosZ() + play->ViewZ}, lvl->planes[i]->normal, lvl->planes[i]->centroid);

		if (f.z != numeric_limits<float>::quiet_NaN())
		{
			vector<Float3> verts = lvl->planes[i]->Vertices;

			bool XY = pointInPolyXY(f.x, f.y, verts);
			bool YZ = pointInPolyYZ(f.y, f.z, verts);
			bool ZX = pointInPolyZX(f.z, f.x, verts);

			if ((XY && YZ) || (YZ && ZX) || (ZX && XY) || (XY && allEqualZ(verts)) || (YZ && allEqualX(verts)) || (ZX && allEqualY(verts)))
			{
				float front = atan2(play->PosY() - f.y, play->PosX() - f.x) - play->GetRadianAngle(play->Angle);

				if (front > M_PI_4 || front < -M_PI_4)
					points.push_back(make_pair(f, lvl->planes[i]));
			}
		}
	}

	sort(points.begin(), points.end(), [play](const pair<Float3, Plane*> a, const pair<Float3, Plane*> b)
	{
		return pow(play->PosX() - a.first.x, 2) + pow(play->PosY() - a.first.y, 2) < pow(play->PosX() - b.first.x, 2) + pow(play->PosY() - b.first.y, 2);
	});

	if (points.size() > 0)
	{
		Float3 dir = {points[0].first.x - play->PosX(), points[0].first.y - play->PosY(), points[0].first.z - play->CamZ()};
		dir = subVectors(dir, scaleVector(0.1f, normalize(dir)));	// The puff must not touch the wall
		//dir = addVectors(dir, scaleVector(0.1f, points[0].second->normal));	// TODO: Use this later when every normal will point inside the level
		lvl->things.push_back(new Puff({play->PosX() + dir.x, play->PosY() + dir.y, play->CamZ() + dir.z}));
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

bool PlayerToPlayersCollision(const Player* source, const vector<Player*> players)
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

vector<Player*> GetPlayersTouched(const Player* source, const vector<Player*> players)
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
