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
	glEnable(GL_DEPTH_TEST);	// Draw objects at the appropriate Z
	glEnable(GL_CULL_FACE);		// Don't draw faces behind polygones

	// Swap buffers right now
	glfwSwapBuffers(window);

	return window;
}

void DrawScreen(GLFWwindow* window, Player* play, Level* lvl)
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	// Tell GL how to show us the world
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	float ratio = (float)width / height;
	float fov = 90;

	// Wide horizontal screen. People with a large screen should not 
	// see more than others. The action is mostly horizontal. 
	if (ratio > 1)
	{
		fov = fov / ratio;
	}

	glViewport(0, 0, width, height);
	gluPerspective(fov, ratio, 0.1f, 100.0f);

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
			glColor3f(235.0f/256.0f, 95.0f/256.0f, 0.0f);
			glutSolidCone(0.08f, 0.5f, 10, 2);
			glPopMatrix();
		}
	}

	// Draw ground
	glColor3f(0.0f, 0.5f, 0.0f);
		glBegin(GL_QUADS);
			glVertex3f(-35.0f, 0.0f, -35.0f);
			glVertex3f(-35.0f, 0.0f, 35.0f);
			glVertex3f(35.0f, 0.0f, 35.0f);
			glVertex3f(35.0f, 0.0f, -35.0f);
		glEnd();
	glPopMatrix();

	// Draw sky
	glColor3f(153.0f / 256.0f, 217.0f / 256.0f, 234.0f / 256.0f);
		glBegin(GL_QUADS);
					// (Xpos, Zpos, Ypos)
			glVertex3f(100.0f, 10.0f, -100.0f);
			glVertex3f(100.0f, 10.0f, 100.0f);
			glVertex3f(-100.0f, 10.0f, 100.0f);
			glVertex3f(-100.0f, 10.0f, -100.0f);
		glEnd();
	glPopMatrix();

	// Draw relative rectangle
	glColor3f(255.0f / 256.0f, 120.0f / 256.0f, 120.0f / 256.0f);
		glBegin(GL_QUADS);
			// (Ypos, Zpos, Xpos)
		glVertex3f(play->PosY / 64 + 10.0f, 5.0f, play->PosX / 64 - 10.0f);
		glVertex3f(play->PosY / 64 + 10.0f, 5.0f, play->PosX / 64 + 10.0f);
		glVertex3f(play->PosY / 64 - 10.0f, 5.0f, play->PosX / 64 + 10.0f);
		glVertex3f(play->PosY / 64 - 10.0f, 5.0f, play->PosX / 64 - 10.0f);
		glEnd();
	glPopMatrix();

	// The sky is blue
	glClearColor(0.0, 0.0, 0.5, 0.0);

	if (lvl != 0)
	{
		for (int i = 0; i < lvl->ptrWalls->size(); i++)
		{
			if (lvl->ptrWalls->at(i).TwoSided)
			{
				glDisable(GL_CULL_FACE);
			}

			glColor3f(0.5f, 0.5f, 0.5f);
			glBegin(GL_QUADS);

			for (int j = 0; j < lvl->ptrWalls->at(i).ptrVertices->size(); j += 3)
			{
				// (Ypos, Zpos, Xpos)
				glVertex3f(lvl->ptrWalls->at(i).ptrVertices->at(j+1), 
					lvl->ptrWalls->at(i).ptrVertices->at(j+2),
					lvl->ptrWalls->at(i).ptrVertices->at(j));
			}

			glEnd();
			glPopMatrix();

			if (lvl->ptrWalls->at(i).TwoSided)
			{
				glEnable(GL_CULL_FACE);
			}
		}
	}
}

int Close_OpenGL()
{
	glfwTerminate();
	return true;
}


