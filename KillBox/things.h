// things.h
//Copyright (C) 2014 Alexandre-Xavier Labonté-Lamoureux
//
//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.

// Data structures (player, barrels, walls, triangles, doors, etc. ) used in the World


#ifndef _THINGS_H
#define _THINGS_H

#include <string>
using namespace std;

const float GRAVITY = 9.8;

struct Player
{
	unsigned short int Angle = 0;

	float PosX;
	float PosY;
	float PosZ;
	int MoX;		//Speed vector (momentum)
	int MoY;
	int MoZ;		//Used by gravity

	const int MaxWalkSpeed = 70;
	const int MaxRunSpeed = 40;

	const int ViewZ = 42;
	char Damages = 0;	//Damage location indicator: 0=none, 1=both, 2=left, 3=right

	int Radius = 16;
	int Height = 56;
	int Health = 100;	//The player's life condition
	int Armor = 100;	//Recharging Energy Shield
	char ArmorClass = 0;

	unsigned int Tags;		//Encode weapons and special proprieties

	int Kills = 0;		//For deathmatch
	int Deaths = 0;

	//Object-Oriented Programming
	void ForwardMove(int Thrust);
	void LateralMove(int Thrust);
	void Fall(/* Map*  */);
};

struct Barrel
{
	float PosX;
	float PosY;
	float PosZ;
	int MoX;
	int MoY;
	int MoZ;		//Used if the barrel falls

	int Radius = 16;
	int Height = 32;
	int Health = 20;

	int State = 0;		//Current state. None, exploding, etc. 
	int Source;		//Could be a pointer on who shot it before its explosion
};

struct Level
{
	string Name = "Unknown";
	string SkyColor;
	string CloudColor;
	string Background = "Black";
	int Fog = 0;

	bool PlayerInGame[4];
	
	//Faire des pointeurs sur des objets, on peut tu faire juste un vecteur à place ?
	////Vertex *ptrWalls = new Vertex[];
	////Vertex *ptrPlayers = new Vertex[];
	////Vertex *ptrSpawns = new Vertex[];
	////Vertex *ptrBarrels = new Vertex[];
	////Vertex *ptrDoors = new Vertex[];
};

struct SpawnSpot
{
	int Xpos;
	int Ypos;
	int Zpos;
	short Angle;
};

struct Vertex
{
	int Xpos;
	int Ypos;
	int Zpos;
};

struct Wall
{
	int Number = 5;
	Vertex *ptr = new Vertex[Number];
	int Xoff = 0;
	int Yoff = 0;
	char Light;
	string Texture;
	int Tag;
};

class VerticalDoor
{
public:
	int Xpos;
	int Ypos;
	float Zpos;		//Current 'Z' position
	float Angle;
	int Ipos;		//Initial 'Z' position
	int Fpos;		//Final 'Z' position

	float Speed = 2;
	bool Closed = true;		//True: Completly closed, False: Open

private:
	float Velocity = 0;
	void Act();		//Do its stuff
};

class NormalDoor
{
public:
	int Xpos;
	int Ypos;
	int Zpos;
	float Angle;
	int Iangle;		//Initial 'Z' angle
	int Fangle;		//Final 'Z' angle

	bool Closed = true;		//True: Completly closed, False: Open

private:
	int Speed = 1;
	float Velocity = 0;
	void Act();		//Do its stuff
};

class HorizontalSlidingDoor
{
public:
	float Xpos;
	int Ypos;
	int Zpos;
	float Angle;
	int Ipos;		//Initial 'Z' angle
	int Fpos;		//Final 'Z' angle

	int Speed = 1;	//Slides on its X axis
	bool Closed = true;		//True: Completly closed, False: Open

private:
	float Velocity = 0;
	void Act();		//Do its stuff
};

#endif _THINGS_H

