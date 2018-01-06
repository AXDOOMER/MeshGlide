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

#ifndef _VIEWDRAW_H
#define _VIEWDRAW_H

#include <GLFW/glfw3.h>

#include <string>
using namespace std;

#include "things.h"
#include "level.h"

const string WindowTitle = "MeshGlide engine";
const string WindowIcon = "planet.png";
const string chars = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
const string fontfile = "chars.png";

void SetWindowTitle(GLFWwindow* window, string Title);

GLFWwindow* Init_OpenGL();

void InitProjection(GLFWwindow* window);

void DrawScreen(GLFWwindow* window, Player* play, Level* lvl, string& FrameDelay);

void Close_OpenGL(GLFWwindow* window);

#endif /* _VIEWDRAW_H */

