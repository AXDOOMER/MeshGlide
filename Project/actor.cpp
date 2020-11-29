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
// actor.cpp
// Data structures for game actors ("things") used in the game world

#include "actor.h"
#include "texture.h"
#include "cache.h"

#include <SDL2/SDL_endian.h>	/* SDL_BYTEORDER, SDL_BIG_ENDIAN */

#include <cmath>
#include <limits>		/* numeric_limits */
#include <string>		/* to_string() */
#include <cstring>		/* memcpy */

using namespace std;

bool Actor::Update()
{
	return true;
}

Weapon::Weapon(float x, float y, float z, const string& type)
{
	pos_.x = x;
	pos_.y = y;
	pos_.z = z;

	Type_ = type;
	Filename_ = Type_ + ".png";

	Cache::Instance()->Add(Filename_, false);
	Radius_ = Cache::Instance()->Get(Filename_)->Width() / 64.0f;
	Height_ = Cache::Instance()->Get(Filename_)->Height() * 2.0f / 64.0f;
}

Weapon::~Weapon()
{
	// Empty
}

float Weapon::PosX() const
{
	return pos_.x;
}

float Weapon::PosY() const
{
	return pos_.y;
}

float Weapon::PosZ() const
{
	return pos_.z;
}

float Weapon::Radius() const
{
	return Radius_;
}

float Weapon::Height() const
{
	return Height_;
}

Texture* Weapon::GetSprite(Float3 /*CamPos*/) const
{
	return Cache::Instance()->Get(Filename_);
}

Puff::Puff(float x, float y, float z)
{
	pos_.x = x;
	pos_.y = y;
	pos_.z = z;

	for (unsigned int i = 0; i < sprites_.size(); i++)
	{
		Cache::Instance()->Add(sprites_[i], false);
	}

	Age_ = 0;
}

Puff::~Puff()
{
	// Empty
}

float Puff::PosX() const
{
	return pos_.x;
}

float Puff::PosY() const
{
	return pos_.y;
}

float Puff::PosZ() const
{
	if (Age_ < 4)
		return pos_.z - 0.05f;

	if (Age_ < 8)
		return pos_.z - 0.10f;

	return pos_.z - 0.10f + ((float)(Age_ - 8) * 0.02f);
}

float Puff::Radius() const
{
	return GetSprite({0,0,0})->Width() / 64.0f;
}

float Puff::Height() const
{
	return GetSprite({0,0,0})->Height() * 2.0f / 64.0f;
}

Texture* Puff::GetSprite(Float3 /*CamPos*/) const
{
	if (Age_ < 4)
		return Cache::Instance()->Get(sprites_[0]);

	if (Age_ < 8)
		return Cache::Instance()->Get(sprites_[1]);

	if (Age_ < 12)
		return Cache::Instance()->Get(sprites_[2]);

	return Cache::Instance()->Get(sprites_[3]);
}

bool Puff::Update()
{
	Age_++;

	if (Age_ < MAX_AGE)
		return true;

	return false;
}

// Blood
Blood::Blood(float x, float y, float z, float groundz)
{
	pos_.x = x;
	pos_.y = y;
	pos_.z = z;
	GroundZ_ = groundz;

	for (unsigned int i = 0; i < sprites_.size(); i++)
	{
		Cache::Instance()->Add(sprites_[i], false);
	}

	Age_ = 0;
	MomZ_ = 0;
}

Blood::~Blood()
{
	// Empty
}

float Blood::PosX() const
{
	return pos_.x;
}

float Blood::PosY() const
{
	return pos_.y;
}

float Blood::PosZ() const
{
	if (Age_ < 8)
		return pos_.z - 0.10f;	// NOTE: Devide the sprite's height by 64 and it should give the same result

	if (Age_ < 14)
		return pos_.z - 0.17f;

	return pos_.z - 0.21;
}

float Blood::Radius() const
{
	return GetSprite({0,0,0})->Width() / 64.0f;
}

float Blood::Height() const
{
	return GetSprite({0,0,0})->Height() * 2.0f / 64.0f;
}

Texture* Blood::GetSprite(Float3 /*CamPos*/) const
{
	if (Age_ < 8)
		return Cache::Instance()->Get(sprites_[0]);

	if (Age_ < 14)
		return Cache::Instance()->Get(sprites_[1]);

	return Cache::Instance()->Get(sprites_[2]);
}

bool Blood::Update()
{
	MomZ_ += GRAVITY * 0.05f;
	Age_++;

	pos_.z -= MomZ_;

	if (pos_.z >= GroundZ_)
		return true;

	return false;
}

Plasma::Plasma(float x, float y, float z, float velx, float vely, float velz)
{
	pos_.x = x;
	pos_.y = y;
	pos_.z = z;

	mom_.x = velx;
	mom_.y = vely;
	mom_.z = velz;

	Cache::Instance()->Add(sprite_, false);

	Age_ = 0;
}

Plasma::~Plasma()
{
	// Empty
}

float Plasma::PosX() const
{
	return pos_.x;
}

float Plasma::PosY() const
{
	return pos_.y;
}

float Plasma::PosZ() const
{
	return pos_.z;
}

float Plasma::Radius() const
{
	return GetSprite({0,0,0})->Width() / 64.0f;
}

float Plasma::Height() const
{
	return GetSprite({0,0,0})->Height() * 2.0f / 64.0f;
}

Texture* Plasma::GetSprite(Float3 /*CamPos*/) const
{
	return Cache::Instance()->Get(sprite_);
}

bool Plasma::Update()
{
	Age_++;

	pos_.x += mom_.x;
	pos_.y += mom_.y;
	pos_.z += mom_.z;

	if (Age_ < MAX_AGE)
		return true;

	return false;
}
