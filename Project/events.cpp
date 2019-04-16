// Copyright (C) 2018-2019 Alexandre-Xavier Labonté-Lamoureux
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
#include "events.h"
#include "player.h"

#include <GLFW/glfw3.h>

#include <fstream>
#include <string>
#include <iostream>
using namespace std;

// Write each player's tic in the demo file
void writeCmdToDemo(ofstream& demo, const vector<Player*>& players)
{
	vector<unsigned char> command;

	for (unsigned int i = 0; i < players.size(); i++)
	{
		command = players[i]->CmdToNet();

		if (command.size() >= BYTES_TO_READ)
		{
			// Write everything except the chat string and its lenght
			demo.write(reinterpret_cast<char*>(command.data()), BYTES_TO_READ);
		}
		else
		{
			cerr << string(__FUNCTION__) << ": Tried to read more bytes than the command size." << endl;
		}
	}
}

// Read a tic from the demo and updates each player
// Returns false if demo must be ended
bool readCmdFromDemo(ifstream& demo, vector<Player*> players)
{
	vector<unsigned char> command;
	command.resize(8, 0);	// '8' because the chat string size is '0'

	// Demo Play is True
	for (unsigned int i = 0; i < players.size(); i++)
	{
		// Read 7 bytes (BYTES_TO_READ) from the demo file and write them to the command vector
		demo.read(reinterpret_cast<char*>(command.data()), BYTES_TO_READ);

		if (!demo)
		{
			cerr << "WARNING: demo ended prematurely" << endl;
			return false;
		}

		players[i]->NetToCmd(command);
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
void updateSpecials(Player*& play, const vector<Player*>& players)
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
