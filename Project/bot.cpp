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

// Simple AI that follows a target
void updateBot(Player* const bot, const Level* const lvl)
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
	}
}
