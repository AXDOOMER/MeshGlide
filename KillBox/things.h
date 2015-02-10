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
#include <cmath>
#include <vector>
using namespace std;

const float GRAVITY = 9.8;
//const float PI = atan(1) * 4;

class Player
{
public:
	// Object-oriented programming is a pain. Made it public so it's easily accessible. 
	short Angle = 8192;	// Should use shorts to avoid a lost of accuracy in MP games...

	float PosX = 0;
	float PosY = 0;
	float PosZ = 0;
	char MoX = 0;		//Speed vector (momentum)
	char MoY = 0;
	char MoZ = 0;		//Used by gravity

private:
	const int MaxWalkSpeed = 40;
	const int MaxRunSpeed = 70;

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


public:
	float GetRadianAngle(short Angle);

	void ForwardMove(int Thrust);
	void LateralMove(int Thrust);
	void AngleTurn(short AngleChange);
	void Fall(/* Map*  */);
};

class Barrel
{
private:
	float PosX;
	float PosY;
	float PosZ;
	int MoX;
	int MoY;
	int MoZ;		//Used if the barrel falls

public:
	int Radius = 16;
	int Height = 32;
	int Health = 20;

	int State = 0;		//Current state. None, exploding, etc. 
	int Source;		//Could be a pointer on who shot it before its explosion
};

struct Wall
{
	vector<int> *ptrVertices = new vector<int>;
	int Xoff = 0;
	int Yoff = 0;
	char Light;
	string Texture;
	int Tag;
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
	vector<Wall> *ptrWalls = new vector<Wall>;
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

