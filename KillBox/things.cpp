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
// things.cpp
// Data structures (player, walls, etc. ) used in the World

#include "things.h"
#include "texture.h"
#include "cache.h"

#include <cmath>
#include <iostream>
using namespace std;

const float PI = atan(1) * 4;

Player::Player()
{
	for (int i = 0; i < MAXOWNEDWEAPONS; i++)
	{
		OwnedWeapons[i] = false;
	}
}

void Player::ForwardMove(int Thrust)
{
	PosX += Thrust * cos(GetRadianAngle(Angle));
	PosY += Thrust * sin(GetRadianAngle(Angle));
}

void Player::LateralMove(int Thrust)
{
	float LateralAngle = GetRadianAngle(Angle) - PI / 2;

	PosX += Thrust * cos(LateralAngle);
	PosY += Thrust * sin(LateralAngle);
}

void Player::ExecuteTicCmd()
{
	ForwardMove(Cmd.forward);
	LateralMove(Cmd.lateral);
	AngleTurn(Cmd.rotation);

	// Empty the tic command
	Cmd.forward = 0;
	Cmd.lateral = 0;
	Cmd.rotation = 0;
	Cmd.fire = 0;
	Cmd.chat = "";
}

float Player::GetRadianAngle(short Angle)
{
	return Angle * PI * 2 / 32768;
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

int Player::Height()
{
	return height;
}

void Player::Height(int newHeight)
{
	height = newHeight;
}

int Player::M()
{
	return PosZ + height / 2;
}

int Player::V()
{
	return height * 4 / 3;
}

int Player::Radius()
{
	return radius;
}

void Player::HealthChange(int Change)
{
	Health = Health + Change;
}

void Level::AddTexture(const string& name)
{
	if (!cache.Has(name))
	{
		cache.Add(name, new Texture(name));
		cout << "Added texture " << name << endl;
	}
}

void Level::UseTexture(const string& name)
{
	cache.Get(name)->Bind();
}
