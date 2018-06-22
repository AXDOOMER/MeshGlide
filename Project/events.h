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
// events.h
// Handles keyboard and mouse events, tic processing (demo and network).

#include "viewdraw.h"
#include "things.h"

#include <GLFW/glfw3.h>

#include <fstream>
using namespace std;

// Write each player's tic in the demo file
void writeCmdToDemo(ofstream& demo, const vector<Player*> players);

// Read a tic from the demo and updates each player
bool readCmdFromDemo(ifstream& demo, vector<Player*> players);

// Takes keyboard and mouse events and applies them to the player
void updatePlayerWithEvents(GLFWwindow* window, GameWindow& view, unsigned int TicCount, Player* play);

// Special updates like the spy key
void updateSpecials(Player*& play, const vector<Player*> players);
