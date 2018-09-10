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
#include "random.h"		/* GetIndex, SetIndex, Seed */
#include "events.h"

#include <GLFW/glfw3.h>
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
	const char* const VERSION = "0.43 (dev)";

	bool Quit = false;
	static unsigned int TicCount = 0;
	bool Debug = false;
	ofstream DemoWrite;
	ifstream DemoRead;
	unsigned int FrameDelay = 0;
	string LevelName = "test.txt";
	bool Fast = false;	// To unlock the speed of the game
	auto GameStartTime = chrono::system_clock::now();
	extern GameWindow view;

	cout << "                MESHGLIDE ENGINE -- " << VERSION << "\n\n";

	/****************************** SYSTEM OPTIONS ******************************/

	if (FindArgumentPosition(argc, argv, "-debug") > 0)
	{
		Debug = true;
		cout << "Debug mode ON." << endl;
	}

	if (FindArgumentPosition(argc, argv, "-time") > 0)
	{
		// Makes the game run as fast as possible and output the time that it took to play the demo at full speed
		Fast = true;
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
		Fast = false;	// Must be false if not playing a demo. It's reserved for demo playback and benchmarking.

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

	if (DemoRead.is_open())
	{
		string line;
		getline(DemoRead, line);
		cout << "Demo Version: " << line << endl;

		if (string(VERSION) != line)
		{
			cout << "Demo is from a different version. A desync may occur." << endl;
		}

		getline(DemoRead, line);
		LevelName = line;
		cout << "Level name: " << line << endl;
		getline(DemoRead, line);
		SetIndex(atoi(line.c_str()));		// Randomization
		cout << "Seed: " << line << endl;
		getline(DemoRead, line);
		cout << "# of players: " << line << endl;
	}
	else
	{
		if (FindArgumentPosition(argc, argv, "-seed") > 0)
		{
			// Accept a parameter that will change the seed
			SetIndex(atoi(FindArgumentParameter(argc, argv, "-seed").c_str()));
		}
		else
		{
			Seed();	// This will change the spawn spot on each start
		}
	}

	unsigned short initialIndex = GetIndex();

	/****************************** OPENGL HANDLING ******************************/

	// Load OpenGL
	GLFWwindow* window = Init_OpenGL(FindArgumentPosition(argc, argv, "-fullscreen") > 0);

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

	LevelName = FindArgumentParameter(argc, argv, "-level", LevelName);

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

	Level* CurrentLevel = new Level(LevelName, atof(FindArgumentParameter(argc, argv, "-scale", "1.0").c_str()));	// Holds the level's data

	if (!CurrentLevel || CurrentLevel->planes.size() == 0)
	{
		cerr << "Failed to load level '" << LevelName << "'" << endl;
		exit(EXIT_FAILURE);
	}

	// Set the players to the floor's height and also set the plane where they are
	for (unsigned int i = 0; i < CurrentLevel->players.size(); i++)
	{
		CurrentLevel->players[i]->plane = GetPlaneForPlayer(CurrentLevel->players[i], CurrentLevel);
		if (CurrentLevel->players[i]->plane == nullptr)
		{
			cerr << "Player's spawn spot is outside of map." << endl;
			exit(EXIT_FAILURE);
		}
	}

	/****************************** SETUP PHASE ******************************/

	if (DemoWrite.is_open())
	{
		DemoWrite << VERSION << endl;
		DemoWrite << LevelName << endl;
		DemoWrite << initialIndex << endl;
		DemoWrite << CurrentLevel->players.size() << endl;	// Number of players TODO: Right now it's hardcoded to 2
	}

	/****************************** GAME LOOP ******************************/
	do
	{
		// Timer
		auto start = chrono::system_clock::now();

		glfwPollEvents();
		RegisterKeyPresses(window);

		if (DemoRead.is_open())
		{
			Quit = !readCmdFromDemo(DemoRead, CurrentLevel->players);
		}
		else
		{
			// Events can only be captured if the player is not in chat mode
			if (!view.chatMode)
			{
				updatePlayerWithEvents(window, view, TicCount, CurrentLevel->play);
			}

			if (DemoWrite.is_open())
			{
				writeCmdToDemo(DemoWrite, CurrentLevel->players);
			}
		}

		updateSpecials(CurrentLevel->play, CurrentLevel->players);

		// Send commands over network and receive commands

		// Update game logic
		for (unsigned int i = 0; i < CurrentLevel->players.size(); i++)
		{
			// Save player's position and the execute the tic command
			Float3 pt = CurrentLevel->players[i]->pos_;
			CurrentLevel->players[i]->ExecuteTicCmd();

			// Handle fire here to avoid circular inclusion/dependecy with 'Level' in the Player class
			if (CurrentLevel->players[i]->ShouldFire)
			{
				Hitscan(CurrentLevel, CurrentLevel->players[i]);
				CurrentLevel->players[i]->ShouldFire = false;
			}

			if (CurrentLevel->play != CurrentLevel->players[i])
			{
				// Player to player collision
				CheckCollision(CurrentLevel->play, CurrentLevel->players[i]);
			}

			// Collision detection with floors and walls
			if (!MovePlayerToNewPosition(pt, CurrentLevel->players[i]->pos_, CurrentLevel->players[i]))
			{
				// Compute the position where the player would be if he slide against the wall
				Float2 pos = MoveOnCollision(pt, CurrentLevel->players[i]->pos_, CurrentLevel->players[i]);

				// Move the player back to its original position
				CurrentLevel->players[i]->pos_ = pt;

				// Try to slide the player against the wall to a valid position
				if (MovePlayerToNewPosition(pt, {pos.x, pos.y, 0}, CurrentLevel->players[i]))
				{
					CurrentLevel->players[i]->pos_.x = pos.x;
					CurrentLevel->players[i]->pos_.y = pos.y;
				}
			}

			ApplyGravity(CurrentLevel->players[i]);
		}

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

		if (!Fast)
		{
			auto end = chrono::system_clock::now();
			FrameDelay = chrono::duration_cast<chrono::microseconds>(end - start).count();

			const int FRAMERATE = 60;
			const float DELAY = 1000 / FRAMERATE;
			if (FrameDelay / 1000 < DELAY)
				SDL_Delay(DELAY - FrameDelay / 1000);
		}

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

	if (Fast)
	{
		// Print benchmark time
		cout << "Game terminated after " << chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - GameStartTime).count() << "ms." << endl;
	}

	// Close OpenGL stuff
	Close_OpenGL(window);

	return EXIT_SUCCESS;
}
