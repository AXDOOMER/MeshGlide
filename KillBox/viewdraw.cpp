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

#include "things.h"

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
	glfwSetKeyCallback(window, Key_Callback);

	// Make the background black
	glClearColor(0.5, 0.5, 0.5, 0.0);
	glEnable(GL_DEPTH_TEST);		// Enable later...
	glEnable(GL_CULL_FACE);

	// Swap buffers right now
	glfwSwapBuffers(window);

	return window;
}

void DrawScreen(Player* play)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, 640, 480);
	gluPerspective(90, 1, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(play->PosY/64, 2.0f, play->PosX / 64,		// Camera's position
		play->PosY / 64 + sin(play->GetRadianAngle(play->Angle)), 2.0f, play->PosX / 64 + cos(play->GetRadianAngle(play->Angle)),	// What the camera will look at
			  0.0, 1.0, 0.0);	// This is for the camera's frame rotation

	glColor3f(1.0f, 1.0f, 1.0f);

	for (int i = -3; i <= 3; i++)
	{
		for (int j = -3; j <= 3; j++)
		{
			glColor3f(1.0f, 1.0f, 1.0f);
			glPushMatrix();
			glTranslatef(i*10.0f, 0.0f, j * 10.0f);
			// Big body snow ball
			glTranslatef(0.0f, 0.75f, 0.0f);
			glutSolidSphere(0.75f, 20, 20);
			// Small head ball
			glTranslatef(0.0f, 1.0f, 0.0f);
			glutSolidSphere(0.25f, 20, 20);
			// Black Eyes
			glPushMatrix();
			glColor3f(0.0f, 0.0f, 0.0f);
			glTranslatef(0.05f, 0.10f, 0.18f);
			glutSolidSphere(0.05f, 10, 10);
			glTranslatef(-0.1f, 0.0f, 0.0f);
			glutSolidSphere(0.05f, 10, 10);
			glPopMatrix();
			// Carrot
			glColor3f(1.0f, 0.5f, 0.0f);
			glutSolidCone(0.08f, 0.5f, 10, 2);
			glPopMatrix();
		}
	}

	//// Draw ground
	glColor3f(0.0f, 0.5f, 0.0f);
	glBegin(GL_QUADS);
	glVertex3f(-35.0f, 0.0f, -35.0f);
	glVertex3f(-35.0f, 0.0f, 35.0f);
	glVertex3f(35.0f, 0.0f, 35.0f);
	glVertex3f(35.0f, 0.0f, -35.0f);
	glEnd();
	glPopMatrix();

	// The sky is blue
	glClearColor(0.0, 0.0, 0.5, 0.0);
}

int Close_OpenGL()
{
	glfwTerminate();
	return true;
}


