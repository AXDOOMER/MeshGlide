// physics.cpp
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

// What affects a thing's movement in space

#include "things.h"

void Teleport(Player &Teleported, float NewPosX, float NewPosY, float NewPosZ, unsigned short NewAngle)
{
	Teleported.PosX = NewPosX;
	Teleported.PosY = NewPosY;
	Teleported.PosZ = NewPosZ;
	Teleported.Angle = NewAngle;

	Teleported.MoX = 0;
	Teleported.MoY = 0;
	Teleported.MoZ = 0;
}

void Fall(Player &Falling)
{
	if (Falling.MoZ == 0)
	{
		Falling.MoZ = 2;
	}
	else
	{
		Falling.MoZ = Falling.MoZ * 2;
	}

}

bool OnGround(Player &Object)
{
	//return PlayerIsOnGround??
	return true;
}

