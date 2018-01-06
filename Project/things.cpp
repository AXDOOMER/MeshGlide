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
#include <limits>		/* numeric_limits<float>::lowest() */
#include <iostream>
#include <algorithm>	/* mismatch */
#include <utility>	/* pair */
using namespace std;

const float PI = atan(1) * 4;

Player::Player()
{
	for (int i = 0; i < MAXOWNEDWEAPONS; i++)
	{
		OwnedWeapons[i] = false;
	}
}

// TODO: Improve the movement system so the division by 64 can be avoided
void Player::ForwardMove(int Thrust)
{
	pos_.x += ((float)Thrust / 64) * cos(GetRadianAngle(Angle));
	pos_.y += ((float)Thrust / 64) * sin(GetRadianAngle(Angle));
}

void Player::LateralMove(int Thrust)
{
	float LateralAngle = GetRadianAngle(Angle) - PI / 2;

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

float Player::PosX()
{
	return pos_.x;
}

float Player::PosY()
{
	return pos_.y;
}

float Player::PosZ()
{
	return pos_.z;
}

Plane::~Plane()
{
	if (WallInfo != nullptr)
	{
		delete WallInfo;
	}
}

void Plane::ComputeWallInfo()
{
	int count = 0;

	// Must find at least two points which are above each other to consider this a wall
	for (int j = 0; j < Vertices.size(); j++)
	{
		for (int k = 0; k < Vertices.size(); k++)
		{
			if (j != k)	// Same, so skip
			{
				if (Vertices[j].x == Vertices[k].x &&
					Vertices[j].y == Vertices[k].y)
				{
					// Above each other
					count++;
				}
			}
		}
	}

	if (count >= 2)
	{
		// Considered a wall
		WallInfo = new Wall;

		WallInfo->Length = 0;
		WallInfo->LowZ = numeric_limits<float>::max();
		WallInfo->HighZ = numeric_limits<float>::lowest();

		for (int j = 0; j < Vertices.size(); j++)
		{
			// Height checks
			if (Vertices[j].z < WallInfo->LowZ)
			{
				WallInfo->LowZ = Vertices[j].z;
			}

			if (Vertices[j].z > WallInfo->HighZ)
			{
				WallInfo->HighZ = Vertices[j].z;
			}

			// Find vertices which are the farthest apart
			for (int k = 0; k < Vertices.size(); k++)
			{
				if (j != k)	// Same, so skip
				{
					float distance = pow(Vertices[j].x - Vertices[k].x, 2) + pow(Vertices[j].y - Vertices[k].y, 2);
					if (distance > WallInfo->Length)
					{
						WallInfo->Length = distance;
						WallInfo->Vertex1 = Vertices[j];
						WallInfo->Vertex2 = Vertices[k];
					}
				}
			}
		}

		// Angle between those two vertices
		WallInfo->Angle = atan2(WallInfo->Vertex1.y - WallInfo->Vertex2.y, WallInfo->Vertex1.x - WallInfo->Vertex2.x);
	}
}
