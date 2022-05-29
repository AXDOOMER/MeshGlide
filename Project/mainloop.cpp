// Copyright (C) 2014-2019 Alexandre-Xavier Labonté-Lamoureux
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
// mainloop.cpp
// The main stuff is here, like the game loop and initialization.

#include "viewdraw.h"
#include "command.h"
#include "actor.h"
#include "player.h"
#include "level.h"
#include "vecmath.h"	/* Float3 */
#include "physics.h"
#include "random.h"		/* GetIndex, SetIndex, Seed */
#include "events.h"
#include "network.h"
#include "strutils.h"	/* Split */
#include "bot.h"

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

int mainloop(int argc, const char* argv[])
{
	const char* const VERSION = "0.59 (dev)";

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
	Network network;
	int numOfPlayers = 1;

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
			throw runtime_error("Could not open demo '" + DemoName + "'");
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
				throw runtime_error("Could not open file '" + DemoName + "' to write");
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
		SetIndex(stoi(line));		// Randomization
		cout << "Seed: " << line << endl;
		getline(DemoRead, line);
		numOfPlayers = stoi(line);
		cout << "# of players: " << line << endl;
	}
	else
	{
		if (FindArgumentPosition(argc, argv, "-seed") > 0)
		{
			// Accept a parameter that will change the seed
			SetIndex(stoi(FindArgumentParameter(argc, argv, "-seed")));
		}
		else
		{
			Seed();	// This will change the spawn spot on each start
		}
	}

	unsigned short initialIndex = GetIndex();

	/****************************** CHOOSE LEVEL ******************************/

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

	/****************************** NETWORKING ******************************/

	if (!DemoRead.is_open())
	{
		string hostport;
		if (FindArgumentPosition(argc, argv, "-host") > 0)
			hostport = FindArgumentParameter(argc, argv, "-host", "5555");

		string serverloc;
		if (FindArgumentPosition(argc, argv, "-connect") > 0)
			serverloc = FindArgumentParameter(argc, argv, "-connect", "localhost:5555");

		if (!hostport.empty())
		{
			numOfPlayers = 2;	// (for now, it's always two players)
			// Start a server
			string info = LevelName + '\n' + to_string(initialIndex) + '\n' + to_string(numOfPlayers);
			network.startServer(hostport, info);
		}
		else if (!serverloc.empty())
		{
			// Or start a client that connects to a server
			string info = network.connectClient(serverloc);
			// Update the game with the info from the server
			vector<string> infos = Split(info, '\n');
			LevelName = infos[0];
			SetIndex(initialIndex = stoi(infos[1]));
			numOfPlayers = stoi(infos[2]);
		}
	}

	/****************************** OPENGL HANDLING ******************************/

	// Load OpenGL
	GLFWwindow* window = Init_OpenGL(FindArgumentPosition(argc, argv, "-fullscreen") > 0, "MeshGlide v" + string(VERSION));

	if (!window)
	{
		throw runtime_error("Could not create OpenGL window!");
	}

	if (FindArgumentPosition(argc, argv, "-wireframe") > 0)
	{
		cout << "_OpenGL: Wireframe mode activated." << endl;
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	/****************************** LEVEL LOADING ******************************/

	Level* CurrentLevel = new Level(LevelName, stof(FindArgumentParameter(argc, argv, "-scale", "1.0")), numOfPlayers);	// Holds the level's data

	if (!CurrentLevel || CurrentLevel->planes.size() == 0)
	{
		throw runtime_error("Failed to load level '" + LevelName + "'");
	}

	/****************************** SETUP PHASE ******************************/

	CurrentLevel->play = CurrentLevel->players[network.myPlayer()];
	CurrentLevel->play->Cmd.id = network.myPlayer();

	if (DemoWrite.is_open())
	{
		DemoWrite << VERSION << endl;
		DemoWrite << LevelName << endl;
		DemoWrite << initialIndex << endl;
		DemoWrite << CurrentLevel->players.size() << endl;
	}

	/****************************** GAME LOOP ******************************/
	do
	{
		// Timer
		auto start = chrono::system_clock::now();

		glfwPollEvents();
		if (!FindArgumentPosition(argc, argv, "-poolonce"))
			glfwPollEvents();
		RegisterKeyPresses(window);

		if (DemoRead.is_open())
		{
			// Read demo. No event capture or network activity occurs.
			Quit = !readCmdFromDemo(DemoRead, CurrentLevel->players);

			if (glfwWindowShouldClose(window))
			{
				Quit = true;
			}
		}
		else
		{
			// Events can only be captured if the player is not in chat mode
			if (!view.chatMode)
			{
				updatePlayerWithEvents(window, view, TicCount, CurrentLevel->play);
			}

			// Run bot on Player 2. For testing.
//			updateBot(CurrentLevel->players[1], CurrentLevel);

			// Cause the game to quit if the player wants to
			if (glfwWindowShouldClose(window))
			{
				CurrentLevel->play->Cmd.quit = true;
			}

			// Send commands over network and receive commands
			if (network.enabled())
			{
				if (network.myPlayer() == 0)
				{
					if (view.chatSend)
					{
						CurrentLevel->players[0]->Cmd.chat = view.chatStr;
						view.chatSend = false;
						view.chatStr.clear();
					}

					// Receive network event from player 2
					CurrentLevel->players[1]->NetToCmd(network.receive());

					// Send network event to player 2
					network.send(CurrentLevel->players[0]->CmdToNet());

					if (CurrentLevel->players[1]->Cmd.chat.size() > 0)
					{
						ShowMessage(view, CurrentLevel->players[1]->Cmd.chat);
					}

				}
				else if (network.myPlayer() == 1)
				{
					if (view.chatSend)
					{
						CurrentLevel->players[1]->Cmd.chat = view.chatStr;
						view.chatSend = false;
						view.chatStr.clear();
					}

					// Send network event to player 1
					network.send(CurrentLevel->players[1]->CmdToNet());

					// Receive network event from player 1
					CurrentLevel->players[0]->NetToCmd(network.receive());

					if (CurrentLevel->players[0]->Cmd.chat.size() > 0)
					{
						ShowMessage(view, CurrentLevel->players[0]->Cmd.chat);
					}
				}
				else
				{
					throw runtime_error("Something is wrong: Player ID is not 0 or 1.");
				}
			}
			else
			{
				// Cleanup the chat strings when in single player mode
				if (view.chatSend)
				{
					view.chatSend = false;
					view.chatStr.clear();
				}
			}

			// Write commands to demo
			if (DemoWrite.is_open())
			{
				writeCmdToDemo(DemoWrite, CurrentLevel->players);
			}
		}

		updateSpecials(CurrentLevel->play, CurrentLevel->players);

		// Update game logic
		for (unsigned int i = 0; i < CurrentLevel->players.size(); i++)
		{
			// Save player's position and the execute the tic command
			Float3 pt = CurrentLevel->players[i]->pos_;
			CurrentLevel->players[i]->ExecuteTick();

			// Collision detection with floors and walls
			if (!NewPositionIsValid(CurrentLevel->players[i], CurrentLevel))
			{
				// Compute the position where the player would be if he slide against the wall
				Float2 pos = MoveOnCollision(pt, CurrentLevel->players[i]->pos_, CurrentLevel->players[i], CurrentLevel);

				// Move the player back to its original position
				// TODO: Shouldn't any 'momentum' be cancelled?
				CurrentLevel->players[i]->pos_ = pt;

				// Try to slide the player against the walls to a valid position
				if (NewPositionIsValid(CurrentLevel->players[i], CurrentLevel))
				{
					// Make sure the walls didn't push the player inside other players
					if (!PlayerToPlayersCollision(CurrentLevel->players[i], CurrentLevel->players))
					{
						// Set the new position
						CurrentLevel->players[i]->pos_.x = pos.x;
						CurrentLevel->players[i]->pos_.y = pos.y;
					}
					else
					{
						// TODO: Shouldn't any 'momentum' be cancelled?
						CurrentLevel->players[i]->pos_ = pt;
					}
				}
			}

			// Player to player collision check
			if (PlayerToPlayersCollision(CurrentLevel->players[i], CurrentLevel->players))
			{
				for (unsigned int j = 0; j < CurrentLevel->players.size(); j++)
				{
					if (CurrentLevel->players[i] != CurrentLevel->players[j])
					{
						// Execute Player to player collision
						CurrentLevel->players[i]->pos_ = PlayerToPlayerCollisionReact(CurrentLevel->players[i], CurrentLevel->players[j]);
						// Check if there's a collision between players
						if (PlayerToPlayerCollision(CurrentLevel->players[i], CurrentLevel->players[j]) ||
							!NewPositionIsValid(CurrentLevel->players[i], CurrentLevel))
						{
							// Restore original position
							CurrentLevel->players[i]->pos_ = pt;
						}
					}
				}
			}

			// Adjust height
			AdjustPlayerToFloor(CurrentLevel->players[i], CurrentLevel);

			// Handle fire here to avoid circular inclusion/dependecy with 'Level' in the Player class
			if (CurrentLevel->players[i]->ShouldFire)
			{
				Hitscan(CurrentLevel, CurrentLevel->players[i], CurrentLevel->players);
				CurrentLevel->players[i]->ShouldFire = false;
			}
		}

		CurrentLevel->UpdateThings();

		// Play sound

		// Status of the player for debugging purposes
		if (Debug)
		{
			cout << "X: " << CurrentLevel->play->PosX() << "\t\tY: " << CurrentLevel->play->PosY()
				<< "\t\tZ: " << CurrentLevel->play->PosZ() << "\t\tA: " << CurrentLevel->play->Angle << endl;
		}

		TicCount++;

		const int FRAMERATE = 60;
		auto FrameTime = std::chrono::milliseconds(1000 / FRAMERATE);
		auto max = GameStartTime + FrameTime * TicCount;

		// Check if we're not running too late
		// Game won't be responsive on horridly slow computers as the frame will never render
		// due to computer being too slow to process the game logic fast enough.
		if (chrono::system_clock::now() < max)
		{
			// Draw Screen
			FrameDelay = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now() - start).count();
			DrawScreen(window, CurrentLevel->play, CurrentLevel, FrameDelay);

			// If there's still time left, wait so game doesn't update crazily fast
			auto end = chrono::system_clock::now();
			if (!Fast && end < max)
			{
				SDL_Delay(chrono::duration_cast<chrono::milliseconds>(max - end).count());
			}
		}

		// Detect OpenGL errors
		GLenum ErrorCode;
		while ((ErrorCode = glGetError()) != GL_NO_ERROR)
		{
			cerr << (const char*)gluErrorString(ErrorCode) << endl;
		}

		// Find a player who quits and terminate the game.
		for (unsigned int i = 0; i < CurrentLevel->players.size(); i++)
		{
			if (!Quit)
			{
				Quit = CurrentLevel->players[i]->Cmd.quit;
			}
		}
	}
	while (!Quit);

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
