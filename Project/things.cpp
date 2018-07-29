// Copyright (C) 2014-2018 Alexandre-Xavier Labont�-Lamoureux
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
#include <string>		/* to_string() */

#include <iostream>	//XXX REMOVE DEBUG

using namespace std;

Player::Player()
{
	plane = nullptr;

	for (int i = 0; i < MAXOWNEDWEAPONS; i++)
	{
		OwnedWeapons[i] = false;
	}

	sprite = new Texture*[8];

	for (int i = 0; i < 8; i++)
		sprite[i] = new Texture("playa" + to_string(i + 1) + ".png", false);
}

Player::~Player()
{
	// Delete the array's content
	for (int i = 0; i < 8; i++)
		delete sprite[i];

	// Delete the array
	delete[] sprite;
}

void Player::Reset()
{
	// Reset vertical view angle
	VerticalAim = 0;

	// Reset momentum
	MoX = 0;
	MoY = 0;
	MoZ = 0;

	// Reset any falls
	AirTime = 0;
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
	AngleLook(Cmd.vertical);
	if (Cmd.fire)
	{
		ShouldFire = true;
	}

	// Empty the tic command
	Cmd.forward = 0;
	Cmd.lateral = 0;
	Cmd.rotation = 0;
	Cmd.vertical = 0;
	Cmd.fire = false;
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

	return sprite[Quotient % 8];
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

	// Create edges
	for (unsigned int i = 0, j = Vertices.size() - 1; i < Vertices.size(); j = i++)
	{
		vector<Float3> Points;

		float angle1ji = atan2(Vertices[i].y - Vertices[j].y, Vertices[i].x - Vertices[j].x);

		float angle2ij = atan2(Vertices[j].y - Vertices[i].y, Vertices[j].x - Vertices[i].x);

		// If an edge is completly vertical, then skip it.
		if (angle1ji != angle2ij)
		{
			cout << "pos:  i(" << Vertices[i].x << ", " << Vertices[i].y << ")  j(" << Vertices[j].x << ", " << Vertices[j].y << ")" << endl;
			cout << "Angle i->j: " << angle2ij << endl;
			cout << "Angle j->i: " << angle1ji << endl;

			float RadiusToUse =  0.5f;

			// Create an edge that exceed both ends of the original edge.
			float StartX = Vertices[i].x + (float) cos(angle1ji) * RadiusToUse;
			float StartY = Vertices[i].y + (float) sin(angle1ji) * RadiusToUse;
			float EndX = Vertices[j].x + (float) cos(angle2ij) * RadiusToUse;
			float EndY = Vertices[j].y + (float) sin(angle2ij) * RadiusToUse;

			// Create one large size
			float StartX1 = StartX + (float) cos(angle1ji + M_PI_2) * RadiusToUse;
			float StartY1 = StartY + (float) sin(angle1ji + M_PI_2) * RadiusToUse;
			float EndX1 = EndX + (float) cos(angle1ji + M_PI_2) * RadiusToUse;
			float EndY1 = EndY + (float) sin(angle1ji + M_PI_2) * RadiusToUse;

			Points.push_back({StartX1, StartY1, Vertices[i].z});
			Points.push_back({EndX1, EndY1, Vertices[j].z});

			// Create the other large size
			float StartX2 = StartX + (float) cos(angle1ji - M_PI_2) * RadiusToUse;
			float StartY2 = StartY + (float) sin(angle1ji - M_PI_2) * RadiusToUse;
			float EndX2 = EndX + (float) cos(angle1ji - M_PI_2) * RadiusToUse;
			float EndY2 = EndY + (float) sin(angle1ji - M_PI_2) * RadiusToUse;

			Points.push_back({StartX2, StartY2, Vertices[i].z});
			Points.push_back({EndX2, EndY2, Vertices[j].z});

			cout << "pos2: i(" << StartX << ", " << StartY << ")  j(" << EndX << ", " << EndY << ")" << endl;
			cout << "pos3: i(" << StartX1 << ", " << StartY1 << ")  j(" << EndX1 << ", " << EndY1 << ")" << endl;
			cout << "pos4: i(" << StartX2 << ", " << StartY2 << ")  j(" << EndX2 << ", " << EndY2 << ")" << endl;

			Edges.push_back({{StartX, StartY, Vertices[i].z}, {EndX, EndY, Vertices[j].z}, 0, Points});
		}
		else
		{
			Edges.push_back({Vertices[i], Vertices[j], 0, Points});
		}

		//Edges.push_back({Vertices[i], Vertices[j], 0, Points});
	}

	cout << "Created " << Edges.size() << " edges." << endl;

	if (Edges.size() != Vertices.size())
	{
		cout << "FUCK" << endl;
		int* a = nullptr;
		int i = *a;
	}
}

Weapon::Weapon(float x, float y, float z, string type)
{
	pos_.x = x;
	pos_.y = y;
	pos_.z = z;

	sprite = new Texture(type + ".png", false);

	Radius_ = sprite->Width() / 64.0f;
	Height_ = sprite->Height() * 2.0f / 64.0f;
}

Weapon::~Weapon()
{
	delete sprite;
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
	return sprite;
}

Puff::Puff(float x, float y, float z)
{
	pos_.x = x;
	pos_.y = y;
	pos_.z = z;

	sprite = new Texture(name_, false);

	Radius_ = sprite->Width() / 64.0f;
	Height_ = sprite->Height() * 2.0f / 64.0f;
}

Puff::~Puff()
{
	delete sprite;
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
	return pos_.z - 0.10f;
}

float Puff::Radius() const
{
	return Radius_;
}

float Puff::Height() const
{
	return Height_;
}

Texture* Puff::GetSprite(Float3 /*CamPos*/) const
{
	return sprite;
}
