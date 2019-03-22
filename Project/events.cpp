// Copyright (C) 2018 Alexandre-Xavier Labonté-Lamoureux
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
// events.cpp
// Handles keyboard and mouse events, tic processing (demo and network).

#include "viewdraw.h"
#include "entity.h"

#include <GLFW/glfw3.h>

#include <fstream>
#include <string>
#include <iostream>
using namespace std;

// Write each player's tic in the demo file
void writeCmdToDemo(ofstream& demo, const vector<Player*> players)
{
	//if (demo.is_open())
	{
		for (unsigned int i = 0; i < players.size(); i++)
		{
			demo << static_cast<int>(players[i]->Cmd.forward) << endl;
			demo << static_cast<int>(players[i]->Cmd.lateral) << endl;
			demo << static_cast<int>(players[i]->Cmd.rotation) << endl;
			demo << static_cast<int>(players[i]->Cmd.vertical) << endl;
			demo << static_cast<int>(players[i]->Cmd.fire) << endl;
		}
	}
}

// Read a tic from the demo and updates each player
// Returns false if demo must be ended
bool readCmdFromDemo(ifstream& demo, vector<Player*> players)
{
	try
	{
		string line;
		// Demo Play is True
		for (unsigned int i = 0; i < players.size(); i++)
		{
			if (!getline(demo, line))
				return false;
			if (!line.empty())
				players[i]->Cmd.forward = stoi(line);
			if (!getline(demo, line))
				return false;
			if (!line.empty())
				players[i]->Cmd.lateral = stoi(line);
			if (!getline(demo, line))
				return false;
			if (!line.empty())
				players[i]->Cmd.rotation = stoi(line);
			if (!getline(demo, line))
				return false;
			if (!line.empty())
				players[i]->Cmd.vertical = stoi(line);
			if (!getline(demo, line))
				return false;
			if (!line.empty())
				players[i]->Cmd.fire = static_cast<bool>(stoi(line));
		}
	}
	catch (...)
	{
		cerr << "Demo file contains invalid data." << endl;
		return false;
	}

	return true;
}

// Takes keyboard and mouse events and applies them to the player
void updatePlayerWithEvents(GLFWwindow* window, GameWindow& view, unsigned int TicCount, Player* play)
{
	// Get Input from Keyboard
	int multiplicator = 1;
	if (KeyPressed(GLFW_KEY_LEFT_SHIFT) || KeyPressed(GLFW_KEY_RIGHT_SHIFT))
	{
		multiplicator *= 2;
	}

	// Input for tests
	if (!((KeyPressed(GLFW_KEY_W) || KeyPressed(GLFW_KEY_UP))
		&& (KeyPressed(GLFW_KEY_S) || KeyPressed(GLFW_KEY_DOWN))))
	{
		if (KeyPressed(GLFW_KEY_W) || KeyPressed(GLFW_KEY_UP))
		{
			play->Cmd.forward = 10 * multiplicator;
		}
		if (KeyPressed(GLFW_KEY_S) || KeyPressed(GLFW_KEY_DOWN))
		{
			play->Cmd.forward = -10 * multiplicator;
		}
	}
	if (!(KeyPressed(GLFW_KEY_A) && KeyPressed(GLFW_KEY_D)))
	{
		if (KeyPressed(GLFW_KEY_A))
		{
			play->Cmd.lateral = -10 * multiplicator;
		}
		if (KeyPressed(GLFW_KEY_D))
		{
			play->Cmd.lateral = 10 * multiplicator;
		}
	}
	if (!(KeyPressed(GLFW_KEY_LEFT) && KeyPressed(GLFW_KEY_RIGHT)))
	{
		if (KeyPressed(GLFW_KEY_LEFT))
		{
			play->Cmd.rotation = 200 * multiplicator;
		}
		if (KeyPressed(GLFW_KEY_RIGHT))
		{
			play->Cmd.rotation = -200 * multiplicator;
		}
	}

	if (KeyPressed(GLFW_KEY_END))
	{
		play->Cmd.vertical = play->AmountToCenterLook();
	}
	else if (!(KeyPressed(GLFW_KEY_PAGE_UP) && KeyPressed(GLFW_KEY_PAGE_DOWN)))
	{
		if (KeyPressed(GLFW_KEY_PAGE_UP))
		{
			play->Cmd.vertical = 200 * multiplicator;
		}
		if (KeyPressed(GLFW_KEY_PAGE_DOWN))
		{
			play->Cmd.vertical = -200 * multiplicator;
		}
	}

	// Handles mouselook
	if (view.mouseLook)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		glfwSetCursorPos(window, 0, 0);

		if (TicCount > 0 && GetKeyPressCount(GLFW_KEY_F1) != 1 && GetKeyPressCount(GLFW_KEY_F4) == 0 && view.justChanged >= 2)
		{
			if (xpos != 0)
			{
				play->Cmd.rotation = -20.0f * xpos;
			}
			if (ypos != 0)
			{
				play->Cmd.vertical = -20.0f * ypos;
			}
		}

		// Used in order to fix the look moving because the player changed the screen mode (fullscreen <--> windowed mode)
		view.justChanged++;
	}

	if (view.mouseLook && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
	{
		play->Cmd.fire = true;
	}
}

// Changes the player by changing where the pointer is pointing at
void updateSpecials(Player*& play, const vector<Player*> players)
{
	// Spy cam. Take control of another player.
	if (GetKeyPressCount(GLFW_KEY_F12) == 1)
	{
		// Find the index of the current player in the array of players
		unsigned int i = 0;
		while (i < players.size())
		{
			if (players[i] == play)
				break;
			i++;
		}

		// Get the next player
		i = (i + 1) % players.size();
		// Change to that player.
		play = players[i];
		cout << "F12: Took control of player #" << i + 1 << endl;
	}
}
