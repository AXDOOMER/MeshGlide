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
// player.h
// Data structures for game entities (player, things, etc. ) used in the game world

#ifndef PLAYER_H
#define PLAYER_H

#include "texture.h"
#include "vecmath.h"	// Custom library for vector math, collision with planes, etc.
#include "plane.h"
#include "actor.h"
#include "ticcmd.h"

#include <string>
#include <cmath>
#include <vector>

using namespace std;

class Player: public Actor
{
public:
	Ticcmd Cmd = Ticcmd();

	// Object-oriented programming is a pain. Made it public so it's easily accessible.
	short Angle = 8192;	// Yaw
	float VerticalAim = 0;	// Pitch

//	Float3 pos_;		// Position of player's feet
	const float ViewZ = 1.8f;
	char MoX = 0;		// Speed vector (momentum)
	char MoY = 0;
	char MoZ = 0;		// Used by gravity
	const float MaxStep = 1.0f;
	const float Radius_ = 0.5f;
	const float Height_ = 2.0f;

	int AirTime = 0;	// When the player falls
	bool ShouldFire = false;
	int TimeSinceLastShot = 0;

	// Weapons that are in the player's possession
	bool OwnedWeapons[MAXOWNEDWEAPONS];

	// Ammo types
	short Ammo = 0;
	short Shells = 0;
	short Rockets = 0;
	short Cells = 0;

	Player(); // We need a constructor for the weapons array
	~Player();	// Deletes the sprite
	void Reset();
	float PosX() const;
	float PosY() const;
	float PosZ() const;

	// Sprite names
	const vector<string> sprites_ = {"playa1.png", "playa2.png", "playa3.png", "playa4.png", "playa5.png", "playa6.png", "playa7.png", "playa8.png"};
	Texture* GetSprite(Float3 CamPos) const;

private:
	const float MaxWalkSpeed = 0.6f;
	const float MaxRunSpeed = 1.1f;

	int Kills = 0;		// For deathmatch
	int Deaths = 0;

public:
	// Executes the player's actions
	void ExecuteTick();

	// Command transfer
	vector<unsigned char> CmdToNet() const;
	void NetToCmd(vector<unsigned char> v);

	float GetRadianAngle(short Angle) const;
	float Radius() const;
	float Height() const;

	void ForwardMove(int Thrust);
	void LateralMove(int Thrust);
	void AngleTurn(short AngleChange);
	void AngleLook(short AngleChange);
	short AmountToCenterLook();	// Angle

	// Camera position
	float CamX() const;
	float CamY() const;
	float CamZ() const;
	// Camera orientation
	float AimX() const;
	float AimY() const;
	float AimZ() const;

	// TODO: Add Pos() and Aim(). Aim is a normalized vector.
};

struct SpawnSpot
{
	Float3 pos_;
	short Angle;
};

#endif /* PLAYER_H */
