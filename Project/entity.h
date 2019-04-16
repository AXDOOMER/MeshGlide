// Copyright (C) 2014-2019 Alexandre-Xavier Labonté-Lamoureux
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
// entity.h
// Data structures for game entities (things, etc. ) used in the game world

#ifndef ENTITY_H
#define ENTITY_H

#include "texture.h"
#include "vecmath.h"	// Custom library for vector math, collision with planes, etc.
#include "plane.h"

#include <string>
#include <cmath>
#include <vector>

using namespace std;

const float GRAVITY = 0.2f;
//const float PI = atan(1) * 4;
const int MAXOWNEDWEAPONS = 10;

class Entity
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

	virtual bool Update();	// Returns 'true' if still alive, 'false' if it needs to be deleted.

	// So the compiler doesn't warn on deleting an object of polymorphic class type
	// https://stackoverflow.com/questions/353817/should-every-class-have-a-virtual-destructor
	virtual ~Entity() = default;

//protected:
//	Texture* sprite;
	Float3 pos_;	// Position
	Float3 mom_;	// Momentum
	//float Radius;
	Plane* plane;
};

class Weapon: public Entity
{
public:
	Weapon(float x, float y, float z, const string& type);
	~Weapon();	// Deletes the sprite

	float PosX() const;
	float PosY() const;
	float PosZ() const;
	float Radius() const;
	float Height() const;

	string Type_;
	string Filename_;	// For the sprite
	float Radius_;
	float Height_;

	Texture* GetSprite(Float3 CamPos) const;
};

class Puff: public Entity
{
public:
	Puff(float x, float y, float z);
	~Puff();	// Deletes the sprite

	float PosX() const;
	float PosY() const;
	float PosZ() const;
	float Radius() const;
	float Height() const;

	string Type;
	int Age_;
	const int MAX_AGE = 16;

	// Sprite names
	const vector<string> sprites_ = {"puffa0.png", "puffb0.png", "puffc0.png", "puffd0.png"};
	Texture* GetSprite(Float3 CamPos) const;
	bool Update();
};

#endif /* ENTITY_H */
