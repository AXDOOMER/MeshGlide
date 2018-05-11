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
// things.h
// Data structures (player, polygons A.K.A. "planes", etc. ) used in the World

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

class Plane
{
public:
	string Texture;
	bool Impassable = true;
	bool TwoSided = false;
	vector<Float3> Vertices;
	float Xscale = 0;
	float Yscale = 0;
	float Xoff = 0;
	float Yoff = 0;
	float Light = 1;	// Must be between 0 (dark) and 1 (full bright)

	Float3 normal;
	Float3 centroid;
	vector<Plane*> Neighbors;	// List of adjacent planes

	void Process();		// Find centroid, find normal...
	unsigned int CommonVertices(Plane* plane);
};

class TicCmd
{
public:
	unsigned char id;
	signed char forward;
	signed char lateral;
	short rotation;
	bool fire;
	string chat;
};

class Thing
{
public:
	// Camera position
/*	virtual float CamX() const = 0;
	virtual float CamY() const = 0;
	virtual float CamZ() const = 0;
	// Camera orientation
	virtual float AimX() const = 0;
	virtual float AimY() const = 0;
	virtual float AimZ() const = 0;*/

	// Position
	virtual float PosX() const = 0;
	virtual float PosY() const = 0;
	virtual float PosZ() const = 0;

	virtual float Radius() const = 0;
	virtual float Height() const = 0;

	virtual Texture* GetSprite(Float3 CamPos) const = 0;

	// So the compiler doesn't warn on deleting an object of polymorphic class type
	virtual ~Thing() = default;

//protected:
//	Texture* sprite;
	Float3 pos_;	// Position
	Float3 mom_;	// Momentum
	//float Radius;
	Plane* plane;
};

class Player: public Thing
{
public:
	TicCmd Cmd = TicCmd();

	// Object-oriented programming is a pain. Made it public so it's easily accessible.
	short Angle = 8192;	// Yaw
	float VerticalAim = 0;	// Pitch
	float Roll = 0;	// Roll

//	Float3 pos_;		// Position of player's feet
	const float ViewZ = 1.8f;
	char MoX = 0;		// Speed vector (momentum)
	char MoY = 0;
	char MoZ = 0;		// Used by gravity
	const float MaxStep = 1.5f;
	const float Radius_ = 0.5f;
	const float Height_ = 2.0f;

	int AirTime = 0;
	bool Jump = false;
	bool Fly = false;

	// Weapons that are in the player's possession
	bool OwnedWeapons[MAXOWNEDWEAPONS];

	// Ammo types
	short Ammo = 0;
	short Shells = 0;
	short Rockets = 0;
	short Cells = 0;

	Player(); // We need a constructor for the weapons array
	~Player();	// Deletes the sprite
	void Reset();
	float PosX() const;
	float PosY() const;
	float PosZ() const;

	Texture* GetSprite(Float3 CamPos) const;
	Texture** sprite;

private:
	const float MaxWalkSpeed = 0.6f;
	const float MaxRunSpeed = 1.1f;

	int Kills = 0;		// For deathmatch
	int Deaths = 0;

public:
	void ExecuteTicCmd();
	float GetRadianAngle(short Angle) const;
	float Radius() const;
	float Height() const;

	void ForwardMove(int Thrust);
	void LateralMove(int Thrust);
	void AngleTurn(short AngleChange);

	// Camera position
	float CamX() const;
	float CamY() const;
	float CamZ() const;
	// Camera orientation
	float AimX() const;
	float AimY() const;
	float AimZ() const;
};

class Weapon: public Thing
{
public:
	Weapon(float x, float y, float z, string type);
	~Weapon();	// Deletes the sprite

	float PosX() const;
	float PosY() const;
	float PosZ() const;
	float Radius() const;
	float Height() const;

	string Type;
	float Radius_;
	float Height_;

	Texture* sprite;
	Texture* GetSprite(Float3 CamPos) const;
};

class Critter: public Thing
{
private:
	float Direction = 0;
public:
	void Move();
};

struct SpawnSpot
{
	Float3 pos_;
	short Angle;
};

#endif /* THINGS_H */
