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
// things.cpp
// Data structures (player, walls, etc. ) used in the World

#include "things.h"
#include "texture.h"
#include "cache.h"

#include <cmath>
#include <limits>		/* numeric_limits */

using namespace std;

Player::Player()
{
	plane = nullptr;

	for (int i = 0; i < MAXOWNEDWEAPONS; i++)
	{
		OwnedWeapons[i] = false;
	}
}

void Player::Reset()
{
	// Reset vertical view angle
	VerticalAim = 0;

	// Reset momentum
	MoX = 0;
	MoY = 0;
	MoZ = 0;

	// Reset any jumping
	AirTime = 0;
	Jump = false;
	Fly = false;
}

// TODO: Improve the movement system so the division by 64 can be avoided
void Player::ForwardMove(int Thrust)
{
	pos_.x += ((float)Thrust / 64) * cos(GetRadianAngle(Angle));
	pos_.y += ((float)Thrust / 64) * sin(GetRadianAngle(Angle));
}

void Player::LateralMove(int Thrust)
{
	float LateralAngle = GetRadianAngle(Angle) - M_PI / 2;

	pos_.x += ((float)Thrust / 64) * cos(LateralAngle);
	pos_.y += ((float)Thrust / 64) * sin(LateralAngle);
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
	return pos_.x + cos(GetRadianAngle(Angle));
}

float Player::AimY() const
{
	return pos_.y + sin(GetRadianAngle(Angle));
}

float Player::AimZ() const
{
	return pos_.z + ViewZ + VerticalAim;
}

// Used to compare two planes by counting the amount of common vertices
unsigned int Plane::CommonVertices(Plane* plane)
{
	unsigned int count = 0;

	for (unsigned i = 0; i < Vertices.size(); i++)
		for (unsigned j = 0; j < plane->Vertices.size(); j++)
			if (Vertices[i] == plane->Vertices[j])
				count++;

	return count;
}

// Process a plane
void Plane::Process()
{
	normal = ComputeNormal(Vertices);
	centroid = ComputeAverage(Vertices);
}
