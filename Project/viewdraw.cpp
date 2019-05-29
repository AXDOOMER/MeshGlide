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
// viewdraw.cpp
// OpenGL camera functions for screen drawing

#include "viewdraw.h"
#include "actor.h"
#include "player.h"
#include "level.h"
#include "vecmath.h" // Float3

#include <SDL2/SDL_image.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <cstdlib>	// EXIT_FAILURE

#include <iostream>	// cerr, endl
#include <string>
#include <cmath>
#include <algorithm>	// sort()
#include <regex>	// regex_replace()
using namespace std;

GameWindow view;

void Key_Callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (view.chatMode && view.chatStr.size() > 0 && key == GLFW_KEY_BACKSPACE && (action == GLFW_REPEAT || action == GLFW_PRESS))
		view.chatStr.erase(view.chatStr.size() - 1);

	if (view.chatMode && key == GLFW_KEY_ENTER && action == GLFW_PRESS)
	{
		view.chatSend = true;
		view.chatMode = false;
	}

	if (!view.chatMode && key == GLFW_KEY_T && action == GLFW_RELEASE)
		view.chatMode = true;

	// Disable or enable mouse using a key
	if (!view.fullScreen)
	{
		if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
		{
			view.mouseLook = !view.mouseLook;

			if (view.mouseLook)
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	// Hide cursor
			}
			else
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				glfwSetCursorPos(window, view.width / 2, view.height / 2);
			}
		}
	}

	// Fullscreen and windowed mode
#if GLFW_VERSION_MINOR >= 2
	if (key == GLFW_KEY_F4 && action == GLFW_PRESS)
	{
		if (view.fullScreen)
		{
			const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
			// Compute window position from fullscreen resolution
			const int xpos = mode->width / 2 - view.width / 2;
			const int ypos = mode->height / 2 - view.height / 2;
			glfwSetWindowMonitor(window, NULL, xpos, ypos, view.width, view.height, 0);
			view.fullScreen = false;

			view.justChanged = 0;
		}
		else
		{
			const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
			glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
			view.fullScreen = true;

			// Always enable mouse look in fullscreen mode
			view.mouseLook = true;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	// Hide cursor

			view.justChanged = 0;
		}
	}
#endif
}

// This function updates the count of how long keys have been pressed.
void RegisterKeyPresses(GLFWwindow* window)
{
	// Cycle from the first key to the last key
	for (int i = GLFW_KEY_SPACE; i <= GLFW_KEY_LAST; i++)
	{
		if (glfwGetKey(window, i))
			view.KeyPresses[i]++;
		else
			view.KeyPresses[i] = 0;
	}
}

// Returns a count of the time that a key has been pressed. Returns -1 if the key is invalid.
int GetKeyPressCount(int key)
{
	if (key >= GLFW_KEY_SPACE && key <= GLFW_KEY_LAST)
		return view.KeyPresses[key];

	return -1;
}

// Returns true if the specified key is pressed
bool KeyPressed(int key)
{
	if (key >= GLFW_KEY_SPACE && key <= GLFW_KEY_LAST)
		return view.KeyPresses[key] > 0;

	return 0;
}

void Char_Callback(GLFWwindow* /*window*/, unsigned int codepoint)
{
	if (view.chatMode && codepoint >= ' ' && codepoint <= '~' && view.chatStr.size() < 36)
		view.chatStr += static_cast<unsigned char>(codepoint);
}

void WindowResize_Callback(GLFWwindow* /*window*/, int width, int height)
{
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

	glViewport(0.0f, 0.0f, width, height);
	gluPerspective(fov, ratio, 0.01f, 200.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Update view
	if (!view.fullScreen)
	{
		view.width = width;
		view.height = height;
	}
}

void Error_Callback(int /*error*/, const char* description)
{
	cerr << description << endl;
}

void SetWindowTitle(GLFWwindow* window, string Title)
{
	glfwSetWindowTitle(window, Title.c_str());
}

GLFWwindow* Init_OpenGL(const bool fullscreen, const string& title)
{
	// Create the window
	GLFWwindow* window;

	glfwSetErrorCallback(Error_Callback);

	// Initialise the glfw library
	if (!glfwInit())
		exit(EXIT_FAILURE);

	// Create a window and its OpenGL context. The window defaults to windowed mode, but can be made fullscreen.
	if (!fullscreen)
	{
		window = glfwCreateWindow(view.width, view.height, title.c_str(), NULL, NULL);
	}
	else
	{
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		window = glfwCreateWindow(mode->width, mode->height, title.c_str(), glfwGetPrimaryMonitor(), NULL);

		view.fullScreen = view.mouseLook = true;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	if (!window)
	{
		// If it didn't work
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// Make its context current
	glfwMakeContextCurrent(window);

	// Set icon. Feature was added to GLFW 3.2
#if GLFW_VERSION_MINOR >= 2
	GLFWimage icon;
	SDL_Surface* Surface = IMG_Load(WindowIcon.c_str());
	if (Surface != nullptr)
	{
		icon.width = Surface->w;
		icon.height = Surface->h;
		icon.pixels = (unsigned char*)Surface->pixels;
		glfwSetWindowIcon(window, 1, &icon);
		SDL_FreeSurface(Surface);
	}
#endif

	// Vsync?
	glfwSwapInterval(0);	// If it's not zero, the framerate is at least limited to the screen's refresh rate.

	// Set important stuff
	glfwSetKeyCallback(window, Key_Callback);
	glfwSetCharCallback(window, Char_Callback);
	InitProjection(window);

	// Set callback
	glfwSetWindowSizeCallback(window, WindowResize_Callback);

	// Make the background gray
	glClearColor(80.0f/256.0f, 119.0f/256.0f, 157.0f/256.0f, 0.0);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);		// Draw objects at the appropriate Z
	glEnable(GL_CULL_FACE);		// Don't draw faces behind polygons

	return window;
}

void InitProjection(GLFWwindow* window)
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	// Tell GL how to show us the world
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// This sets the matrices for the window
	WindowResize_Callback(window, width, height);
}

void RenderText(Level* lvl, string text, float x, float y, float sx, float sy)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluOrtho2D(-1, 1, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Init font texture
	lvl->AddTexture(fontfile, false);

	// Bind the texture that has the fonts
	lvl->UseTexture(fontfile);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);	// Reset the texture colorization to be neutral

	for (unsigned int p = 0, pl = 0; p < text.size(); p++, pl++)
	{
		unsigned char letter = text[p];

		if (letter == '\n')
		{
			y -= 0.10f;
			pl = -1;		// Restart the rendering of text at the left margin of the screen
			continue;
		}

		if (letter >= '!' && letter <= '~')
		{
			letter = letter - '!';	// ! == letter[0]

			// Shift the texture coordinates over the texture to show different letters
			const float dim = 1056.0f / 96.0f / 1036.0f;	// ~ 0.0106235521236f
			float shift = dim * letter;
			float pos = pl * sx;

			glPushMatrix();
				glBegin(GL_QUADS);
					glTexCoord2d(0.0f + shift, 1.0f);
					glVertex2d(x + pos, y - sy);

					glTexCoord2d(0.01f + shift, 1.0f);
					glVertex2d(x + sx + pos, y - sy);

					glTexCoord2d(0.01f + shift, 0.0f);
					glVertex2d(x + sx + pos, y);

					glTexCoord2d(0.0f + shift, 0.0f);
					glVertex2d(x + pos, y);
				glEnd();
			glPopMatrix();
		}
	}
}

void DrawCursor(Level* lvl)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluOrtho2D(-1, 1, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Init font texture
	lvl->AddTexture("crosshair.png", false);

	// Bind the texture that has the fonts
	lvl->UseTexture("crosshair.png");

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);	// Reset the texture colorization to be neutral

	glPushMatrix();
		glBegin(GL_QUADS);
			glTexCoord2f(0.0, 0.0);
			glVertex2f(-0.1f, -0.1f);

			glTexCoord2f(1.0, 0.0);
			glVertex2f(0.1f, -0.1f);

			glTexCoord2f(1.0, 1.0);
			glVertex2f(0.1f, 0.1f);

			glTexCoord2f(0.0, 1.0);
			glVertex2f(-0.1f, 0.1f);
		glEnd();
	glPopMatrix();
}

// Render the screen. Convert in-game axes system to OpenGL axes. (X,Y,Z) becomes (Y,Z,X).
void DrawScreen(GLFWwindow* window, Player* play, Level* lvl, unsigned int FrameDelay)
{
	// Reset colors and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Load identity matrix
	glLoadIdentity();

	// Enable textures
	glEnable(GL_TEXTURE_2D);

	float HorizontalRotation = play->GetRadianAngle(play->Angle);
	// Rotate the player in order to look left and right
	glRotatef(HorizontalRotation * (180.0f / M_PI) + 180.0f, 0, -1, 0);

	// Look up and down
	glRotatef(play->VerticalAim * (180.0f / M_PI), sin(HorizontalRotation + M_PI_2), 0, cos(HorizontalRotation + M_PI_2));

	// Set the camera to the player's position
	glTranslatef(-play->CamY(), -play->CamZ(), -play->CamX());

	// Enable transparency
	glEnable(GL_BLEND);
	glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
	glAlphaFunc(GL_GREATER, 0.1f);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Check if level is not a null pointer to avoid errors and draw its content
	if (lvl != nullptr)
	{
		if (!lvl->SkyTexture.empty())
		{
			// Draw sky (relative to player)
			lvl->UseTexture(lvl->SkyTexture);
			glColor3f(1.0f, 1.0f, 1.0f);
			glDisable(GL_CULL_FACE);
			glPushMatrix();
				glBegin(GL_QUADS);
					// (Xpos, Zpos, Ypos)
					// TODO: Use this for sky coords: glTranslatef(play->PosY, play->PosZ, play->PosX);
					glTexCoord2f(-1, 1);
					glVertex3f(play->PosY() + lvl->SkyHeigth * 20.0f, play->PosZ() + lvl->SkyHeigth, play->PosX() - lvl->SkyHeigth * 20.0f);
					glTexCoord2f(1, 1);
					glVertex3f(play->PosY() + lvl->SkyHeigth * 20.0f, play->PosZ() + lvl->SkyHeigth, play->PosX() + lvl->SkyHeigth * 20.0f);
					glTexCoord2f(1, -1);
					glVertex3f(play->PosY() - lvl->SkyHeigth * 20.0f, play->PosZ() + lvl->SkyHeigth, play->PosX() + lvl->SkyHeigth * 20.0f);
					glTexCoord2f(-1, -1);
					glVertex3f(play->PosY() - lvl->SkyHeigth * 20.0f, play->PosZ() + lvl->SkyHeigth, play->PosX() - lvl->SkyHeigth * 20.0f);
				glEnd();
			glPopMatrix();
		}

		glClear(GL_DEPTH_BUFFER_BIT);	// Clear depth buffer so the sky will always be drawn behind everything

		// Draw walls
		for (unsigned int i = 0; i < lvl->planes.size(); i++)
		{
			if (!lvl->planes[i]->Texture.empty())
			{
				lvl->UseTexture(lvl->planes[i]->Texture);

				if (lvl->planes[i]->TwoSided)
					glDisable(GL_CULL_FACE);
				else
					glEnable(GL_CULL_FACE);

				glPushMatrix();
				{
					//glTranslatef(0, 0, 0);
					// Light: Could be made RGB tint later
					glColor3f(lvl->planes[i]->Light, lvl->planes[i]->Light, lvl->planes[i]->Light);

					if (lvl->HasUVs())
					{
						// Notice: The Y axis on the texture coordinate is flipped (see: https://halfgeek.org/wiki/Vertically_invert_a_surface_in_SDL)

						if (lvl->planes[i]->Vertices.size() == 4)
						{
							// Polygons that are square
							glBegin(GL_QUADS);
							{
								glTexCoord2f(lvl->planes[i]->UVs[0].x, -lvl->planes[i]->UVs[0].y);
								glVertex3f(lvl->planes[i]->Vertices[0].y, lvl->planes[i]->Vertices[0].z, lvl->planes[i]->Vertices[0].x);
								glTexCoord2f(lvl->planes[i]->UVs[1].x, -lvl->planes[i]->UVs[1].y);
								glVertex3f(lvl->planes[i]->Vertices[1].y, lvl->planes[i]->Vertices[1].z, lvl->planes[i]->Vertices[1].x);
								glTexCoord2f(lvl->planes[i]->UVs[2].x, -lvl->planes[i]->UVs[2].y);
								glVertex3f(lvl->planes[i]->Vertices[2].y, lvl->planes[i]->Vertices[2].z, lvl->planes[i]->Vertices[2].x);
								glTexCoord2f(lvl->planes[i]->UVs[3].x, -lvl->planes[i]->UVs[3].y);
								glVertex3f(lvl->planes[i]->Vertices[3].y, lvl->planes[i]->Vertices[3].z, lvl->planes[i]->Vertices[3].x);
							}
							glEnd();
						}
						else if (lvl->planes[i]->Vertices.size() == 3)
						{
							// Polygons that have a triangular shape
							glBegin(GL_TRIANGLES);
							{
								glTexCoord2f(lvl->planes[i]->UVs[0].x, -lvl->planes[i]->UVs[0].y);
								glVertex3f(lvl->planes[i]->Vertices[0].y, lvl->planes[i]->Vertices[0].z, lvl->planes[i]->Vertices[0].x);
								glTexCoord2f(lvl->planes[i]->UVs[1].x, -lvl->planes[i]->UVs[1].y);
								glVertex3f(lvl->planes[i]->Vertices[1].y, lvl->planes[i]->Vertices[1].z, lvl->planes[i]->Vertices[1].x);
								glTexCoord2f(lvl->planes[i]->UVs[2].x, -lvl->planes[i]->UVs[2].y);
								glVertex3f(lvl->planes[i]->Vertices[2].y, lvl->planes[i]->Vertices[2].z, lvl->planes[i]->Vertices[2].x);
							}
							glEnd();
						}
					}
					else	// No UVs
					{
						if (lvl->planes[i]->Vertices.size() == 4)
						{
							// Polygons that are square
							glBegin(GL_QUADS);
							{
								glTexCoord2f(0, 1 * lvl->planes[i]->Yscale);
								glVertex3f(lvl->planes[i]->Vertices[0].y, lvl->planes[i]->Vertices[0].z, lvl->planes[i]->Vertices[0].x);
								glTexCoord2f(1 * lvl->planes[i]->Xscale, 1 * lvl->planes[i]->Yscale);
								glVertex3f(lvl->planes[i]->Vertices[1].y, lvl->planes[i]->Vertices[1].z, lvl->planes[i]->Vertices[1].x);
								glTexCoord2f(1 * lvl->planes[i]->Xscale, 0);
								glVertex3f(lvl->planes[i]->Vertices[2].y, lvl->planes[i]->Vertices[2].z, lvl->planes[i]->Vertices[2].x);
								glTexCoord2f(0, 0);
								glVertex3f(lvl->planes[i]->Vertices[3].y, lvl->planes[i]->Vertices[3].z, lvl->planes[i]->Vertices[3].x);
							}
							glEnd();
						}
						else if (lvl->planes[i]->Vertices.size() == 3)
						{
							// Polygons that have a triangular shape
							glBegin(GL_TRIANGLES);
							{
								glTexCoord2f(0, 1 * lvl->planes[i]->Yscale);
								glVertex3f(lvl->planes[i]->Vertices[0].y, lvl->planes[i]->Vertices[0].z, lvl->planes[i]->Vertices[0].x);
								glTexCoord2f(1 * lvl->planes[i]->Xscale, 1 * lvl->planes[i]->Yscale);
								glVertex3f(lvl->planes[i]->Vertices[1].y, lvl->planes[i]->Vertices[1].z, lvl->planes[i]->Vertices[1].x);
								glTexCoord2f(1 * lvl->planes[i]->Xscale, 0);
								glVertex3f(lvl->planes[i]->Vertices[2].y, lvl->planes[i]->Vertices[2].z, lvl->planes[i]->Vertices[2].x);
							}
							glEnd();
						}
					}
				}
				glPopMatrix();
			}
		}

		// Sprites are always drawn front-facing
		glDisable(GL_CULL_FACE);

		// Draw "things" on the map
		for (unsigned int i = 0; i < lvl->things.size(); i++)
		{
			lvl->things[i]->GetSprite(lvl->play->pos_)->Bind();

			glPushMatrix();
			{
				//glColor3f(lvl->things[i]->plane->Light, lvl->things[i]->plane->Light, lvl->things[i]->plane->Light);
				glColor3f(1.0f, 1.0f, 1.0f);

				glBegin(GL_QUADS);
				{
					float OrthAngle = play->GetRadianAngle(play->Angle) - M_PI / 2;
					float CosOrth = cos(OrthAngle);
					float SinOrth = sin(OrthAngle);

					glTexCoord2f(1, 0);
					glVertex3f(lvl->things[i]->PosY() + SinOrth * lvl->things[i]->Radius(),
						lvl->things[i]->PosZ() + lvl->things[i]->Height(),
						lvl->things[i]->PosX() + CosOrth * lvl->things[i]->Radius());


					glTexCoord2f(0, 0);
					glVertex3f(lvl->things[i]->PosY() - SinOrth * lvl->things[i]->Radius(),
						lvl->things[i]->PosZ() + lvl->things[i]->Height(),
						lvl->things[i]->PosX() - CosOrth * lvl->things[i]->Radius());

					glTexCoord2f(0, 1);
					glVertex3f(lvl->things[i]->PosY() - SinOrth * lvl->things[i]->Radius(),
						lvl->things[i]->PosZ(),
						lvl->things[i]->PosX() - CosOrth * lvl->things[i]->Radius());

					glTexCoord2f(1, 1);
					glVertex3f(lvl->things[i]->PosY() + SinOrth * lvl->things[i]->Radius(),
						lvl->things[i]->PosZ(),
						lvl->things[i]->PosX() + CosOrth * lvl->things[i]->Radius());
				}
				glEnd();
			}
			glPopMatrix();
		}
	}

	glEnable(GL_CULL_FACE);

	// Render text as the last thing because else it will break the rendering
	RenderText(lvl, regex_replace(to_string((float)FrameDelay / 1000) + " ms", regex("0+(?=\\s)\\b"), ""), -0.9f, 0.8f, 0.05f, 0.15f);
	if (view.chatMode)
		RenderText(lvl, view.chatStr + '_', -0.9f, 0.6f, 0.05f, 0.15f);	// Chat text
	if (view.message.size() > 0 && view.timer > SDL_GetTicks())
	{
		RenderText(lvl, view.message, -0.9f, 0.3f, 0.05f, 0.15f);	// Message
	}

	DrawCursor(lvl);

	// Resetting display to 3D
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	WindowResize_Callback(window, width, height);

	// Swap the front and back buffers
	glfwSwapBuffers(window);
}

void ShowMessage(GameWindow& view, const string& message, const int time)
{
	view.message = message;
	view.timer = SDL_GetTicks() + time;
}

void Close_OpenGL(GLFWwindow* window)
{
	glfwDestroyWindow(window);
	glfwTerminate();
}
