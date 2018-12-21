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
// viewdraw.h
// OpenGL camera functions for screen drawing

#ifndef VIEWDRAW_H
#define VIEWDRAW_H

#include <GLFW/glfw3.h>

#include <string>
using namespace std;

#include "things.h"
#include "level.h"

/****************************** Constants ******************************/

const string WindowIcon = "planet.png";
const string chars = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
const string fontfile = "chars.png";

/****************************** Window ******************************/

struct GameWindow
{
	int width = 640;
	int height = 480;
	int xpos = 0;
	int ypos = 0;
	bool mouseLook = false;
	bool fullScreen = false;
	int justChanged = 0;

	bool chatMode = false;
	string chat;
	bool send = false;

	string message;
	unsigned int timer = 0;
	const unsigned int MESSAGE_TIME = 60 * 5;

	// Array for keypresses. The first 31 items are never changed because they have no corresponding key. This wastes a bit of memory.
	// Use the key handling functions to manipulate.
	int KeyPresses[GLFW_KEY_LAST + 1] = { };
};

/****************************** Key handling ******************************/

void RegisterKeyPresses(GLFWwindow* window);
int GetKeyPressCount(int key);
bool KeyPressed(int key);

/****************************** Other functions ******************************/

void SetWindowTitle(GLFWwindow* window, string Title);

GLFWwindow* Init_OpenGL(const bool fullscreen, const string& windowTitle);

void InitProjection(GLFWwindow* window);

void DrawScreen(GLFWwindow* window, Player* play, Level* lvl, unsigned int FrameDelay);

void Close_OpenGL(GLFWwindow* window);

#endif /* VIEWDRAW_H */

