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
// entity.cpp
// Data structures for game entities (player, things, etc. ) used in the game world

#include "entity.h"
#include "texture.h"
#include "cache.h"

#include <SDL2/SDL_endian.h>	/* SDL_BYTEORDER, SDL_BIG_ENDIAN */

#include <cmath>
#include <limits>		/* numeric_limits */
#include <string>		/* to_string() */
#include <cstring>		/* memcpy */

using namespace std;

bool Entity::Update()
{
	return true;
}

TicCmd::TicCmd()
{
	Reset();
	quit = false;
}

void TicCmd::Reset()
{
	// Init to default values
	forward = 0;
	lateral = 0;
	rotation = 0;
	vertical = 0;
	fire = false;
	chat = "";
}

Player::Player()
{
	plane = nullptr;

	for (int i = 0; i < MAXOWNEDWEAPONS; i++)
	{
		OwnedWeapons[i] = false;
	}

	// Add the sprites to the cache from their filename
	for (unsigned int i = 0; i < sprites_.size(); i++)
	{
		Cache::Instance()->Add(sprites_[i], false);
	}
}

Player::~Player()
{
	// Empty
}

void Player::Reset()
{
	// Reset vertical view angle
	VerticalAim = 0;

	// Reset momentum
	mom_.x = 0;
	mom_.y = 0;
	mom_.z = 0;

	// Reset any falls
	AirTime = 0;
}

// TODO: Improve the movement system so the division by 64 can be avoided
void Player::ForwardMove(int Thrust)
{
	mom_.x += ((float)Thrust / 64) * cos(GetRadianAngle(Angle));
	mom_.y += ((float)Thrust / 64) * sin(GetRadianAngle(Angle));
}

void Player::LateralMove(int Thrust)
{
	float LateralAngle = GetRadianAngle(Angle) - M_PI / 2;

	mom_.x += ((float)Thrust / 64) * cos(LateralAngle);
	mom_.y += ((float)Thrust / 64) * sin(LateralAngle);
}

// Encodes a player's data to a buffer for network usage
vector<unsigned char> Player::CmdToNet() const
{
	// Serialize the command
	vector<unsigned char> c;
	c.resize(8, 0);	// It must be at least 8 bytes long

	c[0] = Cmd.quit;
	c[0] = c[0] << 1;

	c[0] = c[0] | Cmd.fire;
	c[0] = c[0] << 6;

	c[0] = c[0] | Cmd.id;

	c[1] = Cmd.forward;
	c[2] = Cmd.lateral;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	c[3] = Cmd.rotation;
	c[4] = Cmd.rotation >> 8;

	c[5] = Cmd.vertical;
	c[6] = Cmd.vertical >> 8;
#else
	c[3] = Cmd.rotation >> 8;
	c[4] = Cmd.rotation;

	c[5] = Cmd.vertical >> 8;
	c[6] = Cmd.vertical;
#endif

	// Save the chat string's size
	c[7] = Cmd.chat.size();

	// Write chat string to buffer
	for (unsigned int i = 0; i < Cmd.chat.size(); i++)
	{
		c.push_back(Cmd.chat[i]);
	}

	return c;
}

// Decodes a player's data from a buffer and write to command
void Player::NetToCmd(vector<unsigned char> v)
{
	// Safety check if not a least 8 bytes
	if (v.size() < 8)
		return;

	// Deserialize the command
	Cmd.quit = v[0] & 128;
	Cmd.fire = v[0] & 64;
	Cmd.id = v[0] & 63;

	Cmd.forward = v[1];
	Cmd.lateral = v[2];

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	Cmd.rotation = v[4];
	Cmd.rotation <<= 8;
	Cmd.rotation |= v[3];

	Cmd.vertical = v[6];
	Cmd.vertical <<= 8;
	Cmd.vertical |= v[5];
#else
	Cmd.rotation = v[3];
	Cmd.rotation <<= 8;
	Cmd.rotation |= v[4];

	Cmd.vertical = v[5];
	Cmd.vertical <<= 8;
	Cmd.vertical |= v[6];
#endif

	// Empty the chat sting inside the command
	Cmd.chat.clear();

	// Write the chat string to the command
	for (unsigned int i = 0; i < v[7] && i < 36; i++)
	{
		Cmd.chat.push_back(v[i + 8]);
	}
}

void Player::ExecuteTicCmd()
{
	if (AirTime == 0)
	{
		mom_.x = 0;
		mom_.y = 0;

		ForwardMove(Cmd.forward);
		LateralMove(Cmd.lateral);

		pos_.x += mom_.x;
		pos_.y += mom_.y;
	}
	else
	{
		pos_.x += mom_.x;
		pos_.y += mom_.y;
	}

	AngleTurn(Cmd.rotation);
	AngleLook(Cmd.vertical);
	if (Cmd.fire)
	{
		ShouldFire = true;
	}

	// Empty the tic command
	Cmd.Reset();
}

float Player::GetRadianAngle(short Angle) const
{
	return Angle * M_PI * 2 / 32768;
}

void Player::AngleTurn(short AngleChange)
{
	// Our internal representation of angles goes from -16384 to 16383,
	// so there are 32768 different angles possible.

	// If you turn bigger than 180 degrees on one side,
	// why didn't you turn to the other side?
	if (AngleChange < 16383 && AngleChange > -16384)
	{
		Angle += AngleChange;

		if (Angle  > 16383)
		{
			Angle = Angle - 32768;
		}
		else if (Angle < -16384)
		{
			Angle = Angle + 32768;
		}
	}
}

void Player::AngleLook(short AngleChange)
{
	VerticalAim += GetRadianAngle(AngleChange);

	if (VerticalAim > M_PI_2)
	{
		VerticalAim = M_PI_2;
	}
	else if (VerticalAim < -M_PI_2)
	{
		VerticalAim = -M_PI_2;
	}
}

short Player::AmountToCenterLook()
{
	// Amount to center look
	return (short)-(VerticalAim / M_PI * 32768 / 2);
}

float Player::Radius() const
{
	return Radius_;
}

float Player::Height() const
{
	return Height_;
}

float Player::PosX() const
{
	return pos_.x;
}

float Player::PosY() const
{
	return pos_.y;
}

float Player::PosZ() const
{
	return pos_.z;
}

float Player::CamX() const
{
	return pos_.x;
}

float Player::CamY() const
{
	return pos_.y;
}

float Player::CamZ() const
{
	return pos_.z + ViewZ;
}

float Player::AimX() const
{
	return cos(GetRadianAngle(Angle)) * cos(VerticalAim);
}

float Player::AimY() const
{
	return sin(GetRadianAngle(Angle)) * cos(VerticalAim);
}

float Player::AimZ() const
{
	return sin(VerticalAim);
}

Texture* Player::GetSprite(Float3 CamPos) const
{
	// Get the angle between the player thing and the camera
	float Theta = atan2(PosY() - CamPos.y, PosX() - CamPos.x);

	// Adjust the angle so that the correct sprite rotation is shown from the camera's point of view
	Theta = Theta - GetRadianAngle(Angle) + M_PI;

	// Make the angle positive if it's negative so the following code will work fine
	if (Theta < 0)
		Theta += M_PI * 2;

	// Align the middle of a sprite rotation on the middle of an octile
	Theta += M_PI / 8;

	// Cross-multiply and the correct sprite rotation can be retrieved from the array using the quotient
	int Quotient = (Theta * 8) / (M_PI * 2);

	return Cache::Instance()->Get(sprites_[Quotient % 8]);
}

Weapon::Weapon(float x, float y, float z, string type)
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
