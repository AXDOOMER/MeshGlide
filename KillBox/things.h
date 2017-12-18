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
// things.h
// Data structures (player, walls, etc. ) used in the World

#ifndef _THINGS_H
#define _THINGS_H

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
	short Angle = 8192;	// Should use shorts to avoid a lost of accuracy in multiplayer games
	float VerticalAim = 0;

	float PosX = 0;
	float PosY = 0;
	float PosZ = 0;		// Height of player's feet
	const float ViewZ = 2.0f;
	char MoX = 0;		// Speed vector (momentum)
	char MoY = 0;
	char MoZ = 0;		// Used by gravity
	const float MaxStep = 1.5f;

	// Weapons that are in the player's possession
	bool OwnedWeapons[MAXOWNEDWEAPONS];

	// Ammo types
	short Ammo = 0;
	short Shells = 0;
	short Rockets = 0;
	short Cells = 0;

	Player(); // We need a constructor for the weapons array

private:
	const float MaxWalkSpeed = 0.6f;
	const float MaxRunSpeed = 1.1f;

//	const int ViewZ = 42;
	char Damages = 0;	//Damage location indicator: 0=none, 1=both, 2=left, 3=right

	int radius = 16;
	int height = 56;
	int Health = 100;	//The player's life condition
	int Armor = 100;	//Recharging Energy Shield
	char ArmorClass = 0;

	unsigned int Tags;		//Encode weapons and special proprieties

	int Kills = 0;		//For deathmatch
	int Deaths = 0;

public:
	void ExecuteTicCmd();
	float GetRadianAngle(short Angle);

	void ForwardMove(int Thrust);
	void LateralMove(int Thrust);
	void AngleTurn(short AngleChange);
	void Fall(/* Map*  */);

	// Way to do OOP
	int Height();					// Get height
	void Height(int newHeight);		// Set height

	int M();						// Get middle height
	int V();						// Get view height
	int Radius();					// Get radius;
	void HealthChange(int Change);	// Change the player's health
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

struct Wall
{
	// TODO: Compute the 3D orentation of a poly and keep the unit vector data for collision detection
    string Texture;
	bool Impassable = true;
	bool TwoSided = false;
	vector<Float3> Vertices;
	float Xscale = 0;
	float Yscale = 0;
	float Xoff = 0;
	float Yoff = 0;
	float Light = 1;	// Must be between 0 (dark) and 1 (full bright)
};

class Level
{
public:
	Cache<string, Texture> cache;
	vector<Wall> ptrWalls;
	Player* play;
	float scaling = 1.0f;	// Level scaling
	float SkyHeigth = 5.0f;	// Sky elevation
	string SkyTexture;

	void AddTexture(const string& name);	// Add texture to cache if missing
	void UseTexture(const string& name);	// Bind texture
};

struct SpawnSpot
{
	int Xpos;
	int Ypos;
	int Zpos;
	short Angle;
};

#endif /* _THINGS_H */
