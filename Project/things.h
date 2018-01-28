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
// things.h
// Data structures (player, walls, etc. ) used in the World

#ifndef THINGS_H
#define THINGS_H

#include "texture.h"
#include "cache.h"
#include "vecmath.h"	// Custom library for vector math, collision with planes, etc.

#include <string>
#include <cmath>
#include <vector>
using namespace std;

const float GRAVITY = 0.2f;
//const float PI = atan(1) * 4;
const int MAXOWNEDWEAPONS = 10;

struct Wall
{
	float LowZ;	// Lowest point of the wall
	float HighZ;	// Highest point of the wall
	float Length;	// Length from one vertex to the other
	Float3 Vertex1;	// Vertex where the wall starts
	Float3 Vertex2;	// Vertex where the wall ends
	float Angle;	// Angle formed between the vertices
};

class Plane
{
public:
	string Texture;
	bool Impassable = true;
	bool TwoSided = false;
	vector<Float3> Vertices;
	Wall* WallInfo = nullptr;
	float Xscale = 0;
	float Yscale = 0;
	float Xoff = 0;
	float Yoff = 0;
	float Light = 1;	// Must be between 0 (dark) and 1 (full bright)

	Float3 normal;
	vector<Plane*> Neighbors;	// List of adjacent planes

	void ComputeWallInfo();
	unsigned int CommonVertices(Plane* plane);
	~Plane();
};

class TicCmd
{
public:
	unsigned char id;
	char forward;
	char lateral;
	short rotation;
	bool fire;
	string chat;
};

class Player
{
public:
	TicCmd Cmd = TicCmd();

	// Object-oriented programming is a pain. Made it public so it's easily accessible.
	short Angle = 8192;	// Yaw
	float VerticalAim = 0;	// Pitch

	Float3 pos_;		// Position of player's feet
	const float ViewZ = 2.0f;
	char MoX = 0;		// Speed vector (momentum)
	char MoY = 0;
	char MoZ = 0;		// Used by gravity
	const float MaxStep = 1.5f;
	const float Radius = 1.0f;

	int AirTime = 0;
	bool Jump = false;
	bool Fly = false;

	Plane* plane;

	// Weapons that are in the player's possession
	bool OwnedWeapons[MAXOWNEDWEAPONS];

	// Ammo types
	short Ammo = 0;
	short Shells = 0;
	short Rockets = 0;
	short Cells = 0;

	Player(); // We need a constructor for the weapons array
	float PosX() const;
	float PosY() const;
	float PosZ() const;

private:
	const float MaxWalkSpeed = 0.6f;
	const float MaxRunSpeed = 1.1f;

	const float Height = 2.0f;

	int Kills = 0;		// For deathmatch
	int Deaths = 0;

public:
	void ExecuteTicCmd();
	float GetRadianAngle(short Angle) const;

	void ForwardMove(int Thrust);
	void LateralMove(int Thrust);
	void AngleTurn(short AngleChange);
};

class Critter
{
private:
	float PosX = 0;
	float PosY = 0;
	float PosZ = 128;
	float Direction = 0;
public:
	void Move();
};

struct SpawnSpot
{
	int Xpos;
	int Ypos;
	int Zpos;
	short Angle;
};

#endif /* THINGS_H */