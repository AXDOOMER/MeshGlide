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
// main.cpp
// The main stuff is here, like the game loop and initialization.

#include "viewdraw.h"
#include "command.h"
#include "things.h"
#include "level.h"
#include "vecmath.h"	/* Float3 */
#include "physics.h"

#include <GLFW/glfw3.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>	/* SDL_Delay */

#include <iostream>
#include <string>
#include <cstdlib>		/* EXIT_FAILURE, EXIT_SUCCESS */
#include <fstream>
#include <chrono>
using namespace std;

int main(int argc, const char *argv[])
{
	const char* const VERSION = "0.24 (dev)";

	bool Quit = false;
	static unsigned int TicCount = 0;
	static bool Debug = false;
	ofstream DemoWrite;
	ifstream DemoRead;
	string FrameDelay = "";

	cout << "                MESHGLIDE -- " << VERSION << "\n\n";

	/****************************** SYSTEM OPTIONS ******************************/

	if (FindArgumentPosition(argc, argv, "-debug") > 0)
	{
		Debug = true;
		cout << "I_sys: Debug mode ON." << endl;
	}

	/****************************** DEMO FILES ******************************/

	string DemoName = FindArgumentParameter(argc, argv, "-playdemo");
	if (!DemoName.empty())
	{
		cout << "Playing demo: " << DemoName << endl;
		DemoRead.open(DemoName);
		if (!DemoRead.is_open())
		{
			cout << "Could not open demo: " << DemoName << endl;
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		// Playing a demo has priority over recording a demo
		DemoName = FindArgumentParameter(argc, argv, "-record");
		if (!DemoName.empty())
		{
			cout << "Recoring demo: " << DemoName << endl;
			DemoWrite.open(DemoName);
			if (!DemoWrite.is_open())
			{
				cout << "Could not open demo file to write to: " << DemoName << endl;
				exit(EXIT_FAILURE);
			}
		}
	}

	/****************************** OPENGL HANDLING ******************************/

	// Load OpenGL
	GLFWwindow* window = Init_OpenGL();
	// Initialize GLUT, else we can't draw spheres and cones
	glutInit(&argc, const_cast<char**>(argv));

	if (!window)
	{
		cout << "Could not create OpenGL window!" << endl;
		exit(EXIT_FAILURE);
	}

	if (FindArgumentPosition(argc, argv, "-wireframe") > 0)
	{
		cout << "_OpenGL: Wireframe mode activated." << endl;
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	/****************************** LEVEL LOADING ******************************/

	string LevelName = FindArgumentParameter(argc, argv, "-level", "level.txt");

	if (!LevelName.empty())
	{
		cout << "Loading level '" + LevelName << "'" << endl;
	}
	else
	{
		cin.ignore(cin.rdbuf()->in_avail());
		cout << "Enter a level's name: ";
		cin >> LevelName;
	}

	Level* CurrentLevel = new Level(LevelName);	// Holds the level data

	if (!CurrentLevel)
	{
		cout << "ERROR: Cannot load level '" << LevelName << "'" << endl;
		exit(EXIT_FAILURE);
	}

	/****************************** SETUP PHASE ******************************/

	if (DemoWrite.is_open())
	{
		DemoWrite << VERSION << endl;
		DemoWrite << LevelName << endl;
		DemoWrite << 1 << endl;	// Number of players
	}

	if (DemoRead.is_open())
	{
		string line;
		getline(DemoRead, line);
		cout << "Version: " << line << endl;
		getline(DemoRead, line);
		cout << "Level name: " << line << endl;
		getline(DemoRead, line);
		cout << "# of players: " << line << endl;
	}

	/****************************** GAME LOOP ******************************/
    do
    {
		// Timer
		auto start = chrono::system_clock::now();

		glfwPollEvents();

		if (DemoRead.is_open())
		{
			try
			{
				string line;
				// Demo Play is True
				if (!getline(DemoRead, line))
					Quit = true;
				if (!line.empty())
					CurrentLevel->play->Cmd.forward = stoi(line);
				if (!getline(DemoRead, line))
					Quit = true;
				if (!line.empty())
					CurrentLevel->play->Cmd.lateral = stoi(line);
				if (!getline(DemoRead, line))
					Quit = true;
				if (!line.empty())
					CurrentLevel->play->Cmd.rotation = stoi(line);
			}
			catch (...)
			{
				cerr << "ERROR: Demo file contains invalid data." << endl;
				Quit = true;
			}
		}
		else
		{
			// Get Input from Keyboard
			int multiplicator = 1;
			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT))
			{
				multiplicator *= 2;
			}

			// Input for tests
			if (!((glfwGetKey(window, GLFW_KEY_W) || glfwGetKey(window, GLFW_KEY_UP))
				&& (glfwGetKey(window, GLFW_KEY_S) || glfwGetKey(window, GLFW_KEY_DOWN))))
			{
				if (glfwGetKey(window, GLFW_KEY_W) || glfwGetKey(window, GLFW_KEY_UP))
				{
					CurrentLevel->play->Cmd.forward = 10 * multiplicator;
				}
				if (glfwGetKey(window, GLFW_KEY_S) || glfwGetKey(window, GLFW_KEY_DOWN))
				{
					CurrentLevel->play->Cmd.forward = -10 * multiplicator;
				}
			}
			if (!(glfwGetKey(window, GLFW_KEY_A) && glfwGetKey(window, GLFW_KEY_D)))
			{
				if (glfwGetKey(window, GLFW_KEY_A))
				{
					CurrentLevel->play->Cmd.lateral = -10 * multiplicator;
				}
				if (glfwGetKey(window, GLFW_KEY_D))
				{
					CurrentLevel->play->Cmd.lateral = 10 * multiplicator;
				}
			}
			if (!(glfwGetKey(window, GLFW_KEY_LEFT) && glfwGetKey(window, GLFW_KEY_RIGHT)))
			{
				if (glfwGetKey(window, GLFW_KEY_LEFT))
				{
					CurrentLevel->play->Cmd.rotation = 200 * multiplicator;
				}
				if (glfwGetKey(window, GLFW_KEY_RIGHT))
				{
					CurrentLevel->play->Cmd.rotation = -200 * multiplicator;
				}
			}

			if (glfwGetKey(window, GLFW_KEY_END))
			{
				CurrentLevel->play->VerticalAim = 0;
			}
			else if (!(glfwGetKey(window, GLFW_KEY_PAGE_UP) && glfwGetKey(window, GLFW_KEY_PAGE_DOWN)))
			{
				if (glfwGetKey(window, GLFW_KEY_PAGE_UP))
				{
					if (CurrentLevel->play->VerticalAim < 1.0f)
					{
						CurrentLevel->play->VerticalAim += 0.05f;
					}
				}
				if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN))
				{
					if (CurrentLevel->play->VerticalAim > -1.0f)
					{
						CurrentLevel->play->VerticalAim -= 0.05f;
					}
				}
			}

			if (DemoWrite.is_open())
			{
				DemoWrite << static_cast<int>(CurrentLevel->play->Cmd.forward) << endl;
				DemoWrite << static_cast<int>(CurrentLevel->play->Cmd.lateral) << endl;
				DemoWrite << static_cast<int>(CurrentLevel->play->Cmd.rotation) << endl;
			}
		}

		if (glfwGetKey(window, GLFW_KEY_SPACE) || CurrentLevel->play->Jump)
		{
			if (!CurrentLevel->play->Fly)
				CurrentLevel->play->Jump = true;

			if (CurrentLevel->play->Jump)
				CurrentLevel->play->pos_.z = CurrentLevel->play->PosZ() + GRAVITY * 1.5f;
		}

		if (glfwGetKey(window, GLFW_KEY_ESCAPE))
		{
			Quit = true;
		}

		// Send Commands over Network

		// Update game logic
		Float3 pt = {CurrentLevel->play->PosX(), CurrentLevel->play->PosY(), CurrentLevel->play->PosZ()};

		TicCmd tc = CurrentLevel->play->Cmd;
		CurrentLevel->play->ExecuteTicCmd();

		// Collision detection with floors
		if (HitsWall({CurrentLevel->play->PosX(), CurrentLevel->play->PosY(), CurrentLevel->play->PosZ()}, pt, CurrentLevel->play->Radius, CurrentLevel))
		{
			tc.forward = -tc.forward;
			tc.lateral = -tc.lateral;
			CurrentLevel->play->Cmd = tc;
			CurrentLevel->play->ExecuteTicCmd();
		}

		AdjustPlayerToFloor(CurrentLevel->play, CurrentLevel);

		// Draw Screen
		DrawScreen(window, CurrentLevel->play, CurrentLevel, FrameDelay);

		// Play sound

        // Status of the player for debugging purposes
		if (Debug)
		{
			cout << "X: " << CurrentLevel->play->PosX() << "\t\tY: " << CurrentLevel->play->PosY()
				<< "\t\tZ: " << CurrentLevel->play->PosZ() << "\t\tA: " << CurrentLevel->play->Angle << endl;
		}

		TicCount++;

		auto end = chrono::system_clock::now();
		auto diff = chrono::duration_cast<chrono::milliseconds>(end - start).count();
		FrameDelay = to_string(diff);

		const int FRAMERATE = 60;
		const float DELAY = 1000 / FRAMERATE;
		if (diff < DELAY)
			SDL_Delay(DELAY - diff);

		// Detect OpenGL errors
		GLenum ErrorCode;
		while ((ErrorCode = glGetError()) != GL_NO_ERROR)
		{
			cerr << (const char*)gluErrorString(ErrorCode) << endl;
		}
    }
	while (!Quit && !glfwWindowShouldClose(window));

	/****************************** TERMINATION ******************************/

	// Destroy level
	if (CurrentLevel != nullptr)
		delete CurrentLevel;

	if (DemoWrite.is_open())
	{
		DemoWrite.close();
		cout << "Demo written to disk." << endl;
	}

	if (DemoRead.is_open())
	{
		DemoRead.close();
		cout << "Demo playback ended." << endl;
	}

	// Close OpenGL stuff
	Close_OpenGL(window);

	return EXIT_SUCCESS;
}
