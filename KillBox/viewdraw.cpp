// viewdraw.cpp
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

// OpenGL camera functions for screen drawing


#include <GLFW/glfw3.h>
#include <GL/freeglut.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include <iostream>
using namespace std;

void Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void Error_Callback(int error, const char* description)
{
	cout << description << endl;
}

GLFWwindow* Init_OpenGL()
{
	// Create the window
	GLFWwindow* window;


	glfwSetErrorCallback(Error_Callback);

	// Initialise the glfw library
	if (!glfwInit())
		return 0;

	// Create a windowed mode window and its OpenGL context
	window = glfwCreateWindow(640, 480, "KillBox", NULL, NULL);
	if (!window)
	{
		// If it didn't work
		glfwTerminate();
		return 0;
	}

	// Make its context current
	glfwMakeContextCurrent(window);

	// Vsync
	glfwSwapInterval(1);
	glfwSetKeyCallback(window, Key_Callback<);

	// Make the background black
	glClearColor(0.5, 0.5, 0.5, 0.0);
	//glEnable(GL_DEPTH_TEST);		// Enable later...
	glEnable(GL_CULL_FACE);

	// Swap buffers right now
	glfwSwapBuffers(window);

	return window;
}

int Close_OpenGL()
{
	glfwTerminate();
	return true;
}


