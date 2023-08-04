// Copyright (C) 2020 Alexandre-Xavier Labonté-Lamoureux
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
// bot.cpp
// AI bot

#include "player.h"
#include "level.h"

#define _USE_MATH_DEFINES
#include <cmath>

using namespace std;

Player* findTarget(Player* const bot, const Level* const lvl)
{
	// Have at least a bot and a player
	if (lvl->players.size() >= 2)
	{
		for (unsigned int i = 0; i < lvl->players.size(); i++)
		{
			// Target can't be itself
			if (lvl->players[i] != bot)
			{
				return lvl->players[i];
			}
		}
	}

	return nullptr;
}

float angleBetweenPlayers(const Player* const source, const Player* const target)
{
	return atan2(source->PosY() - target->PosY(), source->PosX() - target->PosX());
}

float distance(const Float3& u, const Float3& v)
{
	return sqrt(pow(u.x - v.x, 2) + pow(u.y - v.y, 2));
}

float direction(const Float3& u, const Float3& v)
{
	return atan2(v.y - u.y, v.x - u.x);
}

float velocity(const Float3& v)
{
	return sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
}

// Simple AI that follows a target
void updateBot(Player* const bot, Level* const lvl)
{
	Player* target = findTarget(bot, lvl);

	if (target)
	{
		// Calculate the turn towards the target
		float currentAngle = bot->GetRadianAngle(bot->Angle);

		// Calculate the turn to execute
		int angleDiff = (currentAngle - angleBetweenPlayers(bot, target)) * 32768 / (M_PI * 2) + 16384;

		if (angleDiff > 16383)
		{
			angleDiff -= 32768;
		}
		else if (angleDiff < -16384)
		{
			angleDiff += 32768;
		}

		bot->Cmd.rotation = -angleDiff;

		// Calculate distance from target
		float dist = sqrt(pow(bot->PosX() - target->PosX(), 2) + pow(bot->PosY() - target->PosY(), 2));

		// Move closer if far
		if (dist > 5.0f)
		{
			bot->Cmd.forward = 10;
		}

		// Fire at opponent
		if (bot->TimeSinceLastShot > 30)
		{
			// TODO: LINE OF SIGHT CHECK

			bot->TimeSinceLastShot = 0;

			// Get updated angle
			currentAngle = bot->GetRadianAngle(bot->Angle);

			const int PROJECTILE_SPEED = 1.0f;	// TODO: Has to get the value from the current weapon

			// This code computes an interception point between two objects that move at a different speed
			// Quadratic formula adapted from:
			// https://stackoverflow.com/questions/37250215/intersection-of-two-moving-objects-with-latitude-longitude-coordinates
			float dist = distance(bot->pos_, target->pos_);
			float dir = direction(bot->pos_, target->pos_);

			// "target_dir" is the direction angle of the target
			float target_dir = atan2(target->mom_.y, target->mom_.x);
			float target_vel = velocity(target->mom_);
			// Angle "alpha" is the direction from chaser to target
			float alpha = M_PI + dir - target_dir;
			float chaser_vel = PROJECTILE_SPEED;

			float a = pow(chaser_vel, 2) - pow(target_vel, 2);
			float b = 2 * dist * target_vel * cos(alpha);
			float c = -pow(dist, 2);
			float disc = pow(b, 2) - 4 * a * c;

			// Check if intersection is possible
			if (disc >= 0 && a != 0.0f && dist != 0.0f)
			{
				float time = (sqrt(disc) - b) / (2 * a);
				float x = target->PosX() + target_vel * time * cos(target_dir);
				float y = target->PosY() + target_vel * time * sin(target_dir);

				float missile_zspeed = (target->PosZ() - bot->PosZ()) / dist;
				float missile_dir = direction(bot->pos_, Float3{x, y , 0});
				// Spawn missile
				// TODO: Aim at player if plasma, aim at ground if rocket because of splash damage
				lvl->things.push_back(new Plasma(bot->PosX(), bot->PosY(), bot->CamZ() - 0.5f, cos(missile_dir), sin(missile_dir), missile_zspeed));
			}
			// Else preserve ammo
		}
		else
		{
			bot->TimeSinceLastShot++;
		}

	}
}
