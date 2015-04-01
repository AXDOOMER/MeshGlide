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

#include <cmath>

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

bool PointInPlayer(Player &Object, float PointX, float PointY, float PointZ)
{
	float d2Mesurment = pow(Object.PosX - PointX, 2) + pow(Object.PosY - PointY, 2);
	//float Distance = sqrt(d2Mesurment + pow(Object.PosZ - PointZ, 2));

	if (PointZ > Object.PosZ && Object.PosZ + Object.Height() < PointZ)
	{
		// Thee point is inside the player's Z coordinates

		if (sqrt(d2Mesurment) <= Object.Radius())
		{
			// The point is inside the player's radius
			return true;
		}
	}

	// There was no collision
	return false;
}

void HitScan(Player Players[], int NumberOfPlayers, float IpX, float IpY, float IpZ, float Hangle, float Vangle, int Damage)
{
	float Step = 1;		// Increment betweens tests
	int Max = 2048;		// Max distance we will check in multiples of steps
	bool Hit = false;	// Has the bullet already hit something?

	// Get a position that is farther on the line's direction
	/*int FarX = IpX * 2 * cos(Hangle);
	int FarY = IpY * 2 * sin(Hangle);*/

	// Check for a collision with each players
	/*for (int p = 0; p < NumberOfPlayers; p++)
	{
		// Trace a line that reachers the other player's X position
		int NewX = Players[((p + 1) % 4)].PosX;
		int Multiplyer = Players[p].PosX / IpX;

		// Check if the point is in the player
		if (PointInPlayer(Players[p], IpX, IpY, IpZ))
		{
			// If the point is inside the player, then damage him. 
			Players[p].HealthChange(Damage);
			Hit = true;		// We hit something
			break;
		}
	}*/


	for (int p = 0; p < NumberOfPlayers; p++)
	{
		// The other player's X pos
		int OtherX = Players[((p + 1) % 4)].PosX;
		int LineX = Players[p].PosX;
		int LineY = Players[p].PosY;
		int LineZ = Players[p].PosZ;
		bool Touched = false;

		for (int i = 0; i < Max && !Hit; i++)
		{
			// Move the bullet forward
			LineX += Step * cos(Hangle);
			LineY += Step * sin(Hangle);
			LineZ += Step * sin(Vangle);

			if (LineX >= OtherX - Step || LineX <= OtherX + Step)
			{
				// The line's point is at the same X position as the other player
				Touched = true;	// Yes, the line as touched the other player's X position
				i = Max;

				if (abs(Players[((p + 1) % NumberOfPlayers)].PosY - LineY) <= Players[((p + 1) % NumberOfPlayers)].Radius)
				{
					// The line two-dimensional's point is inside the other player

					if (LineZ >= Players[((p + 1) % NumberOfPlayers)].PosZ && 
						Players[((p + 1) % NumberOfPlayers)].PosZ + Players[((p + 1) % NumberOfPlayers)].Height() <= LineZ)
					{
						// The point is inside the player's Z coordinates, so damage him. 
						Players[((p + 1) % NumberOfPlayers)].HealthChange(Damage);
						Hit = true;		// He was hit
						break;
					}
				}
			}
		}
	}


	

	/*for (int i = 0; i < Max && !Hit; i++)
	{
		// Until we reach the maximum distance to check...

		// TODO: CHECK IF WE HIT A WALL

		for (int p = 0; p < NumberOfPlayers && !Hit; p++)
		{
			// Check if the point is in the player
			if (PointInPlayer(Players[p], IpX, IpY, IpZ))
			{
				// If the point is inside the player, then damage him. 
				Players[p].HealthChange(Damage);
				Hit = true;		// We hit something
				break;
			}
		}

		// Move the bullet forward
		if (!Hit)
		{
			// NOTES: http://www.mathsisfun.com/sine-cosine-tangent.html
			IpX += Step * cos(Hangle);
			IpY += Step * sin(Hangle);
			IpZ += Step * sin(Vangle);
		}
	}*/
}

bool PointPolygonTraversal(Level* Geometry, float PointX, float PointY, float PointZ, float Velocity, float Hangle, float Vangle)
{
	bool Hit = false;

	// Test every wall
	for (unsigned int w = 0; w < Geometry->ptrWalls->size(); w++)
	{
		// Go trough every vertex
		for (unsigned int v = 0; v < Geometry->ptrWalls->at(w).ptrVertices->size(); v++)
		{
			// TODO: CHECK FOR COLISION INSIDE THE POLY
		}
	}

	// If we hit geometry
	return true;
}

// When two players collide together
bool PlayerCollision(Player &One, Player &Two)
{
	// The following takes in account that both players have the same height
	if (abs(One.PosZ - Two.PosZ) <= One.Height())
	{
		// The players are at the same Z height, now test if they are in the same XY space 
		if (pow(One.PosX - Two.PosX, 2) + pow(One.PosY - Two.PosY, 2) <= One.Radius() + Two.Radius())
		{
			// The players are inside each other
			return true;
		}
	}

	// There is no collision
	return false;
}

bool PlayerAgainstSurface(Player Object, Wall Surface)
{

}