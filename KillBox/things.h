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

#include <string>
#include <cmath>
#include <vector>
using namespace std;

const float GRAVITY = 9.81;
//const float PI = atan(1) * 4;
const int MAXOWNEDWEAPONS = 16;

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
	short Angle = 8192;	// Should use shorts to avoid a lost of accuracy in MP games...

	float PosX = 0;
	float PosY = 0;
	float PosZ = 128;
	char MoX = 0;		//Speed vector (momentum)
	char MoY = 0;
	char MoZ = 0;		//Used by gravity

	// Weapons that are in the player's possession
	bool OwnedWeapons[MAXOWNEDWEAPONS] /*= { false, false, false, false, false, false, false, false,
							  false, false, false, false, false, false, false, false }*/;
	// Ammo types
	short Ammo = 0;
	short Shells = 0;
	short Rockets = 0;
	short Cells = 0;

	Player(); // We need a constructor for the weapons array

private:
	const int MaxWalkSpeed = 40;
	const int MaxRunSpeed = 70;

	const int ViewZ = 42;
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

struct Vertex
{
	int Xpos;
	int Ypos;
	int Zpos;
};

struct Wall
{
    string Texture;
	bool Impassable = true;
	bool TwoSided = false;
	vector<Vertex> Vertices;
	char Xscale = 0;
	char Yscale = 0;
	char Xoff = 0;
	char Yoff = 0;
	char Light = 127;
};

struct Level
{
	vector<Wall> ptrWalls;
};

struct SpawnSpot
{
	int Xpos;
	int Ypos;
	int Zpos;
	short Angle;
};

#endif /* _THINGS_H */
