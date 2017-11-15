// Copyright (C) 2014-2017 Alexandre-Xavier Labonté-Lamoureux
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
// The main stuff is here, like the game loop.

// These instructions tell where to put them:
// http://stackoverflow.com/questions/22539047/how-can-i-set-up-freeglut-and-glew-for-use-in-visual-studio-2013
#include <GLFW/glfw3.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <iostream>
#include <string>
#include <string.h>     //Used for strcmp()
#include <stdlib.h>     /* atoi */
#include <fstream>
#include <chrono>

#include "viewdraw.h"
#include "command.h"
#include "things.h"
#include "textread.h"

using namespace std;

int main(int argc, char *argv[])
{
	const char* const VERSION = "0.07 (dev)";		// A serial number for the version

	bool Quit = false;
	static unsigned int TicCount = 0;
	static bool Debug = false;
	ofstream DemoWrite;
	ifstream DemoRead;

	cout << "                KILLBOX -- " << VERSION << "\n\n";

	/****************************** SYSTEM OPTIONS ******************************/

	if (FindArgumentPosition(argc, argv, "-debug") > 0)
	{
		Debug = true;
		cout << "I_sys: Debug mode ON." << endl;
	}

	/****************************** DEMO FILES ******************************/

	string DemoName = FindArgumentParameter(argc, argv, "-playdemo");
	if (DemoName.size() > 0 && DemoName[0] != '-')
	{
		cout << "Playing demo: " << DemoName << endl;
		DemoRead = ifstream(DemoName);
		if (!DemoRead.is_open())
		{
			cout << "Could not open demo: " << DemoName << endl;
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		DemoName = "";
	}

	DemoName = FindArgumentParameter(argc, argv, "-record");
	if (DemoName.size() > 0 && DemoName[0] != '-')
	{
		cout << "Recoring demo: " << DemoName << endl;
		DemoWrite = ofstream(DemoName);
		if (!DemoWrite.is_open())
		{
			cout << "Could not open demo file to write to: " << DemoName << endl;
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		DemoName = "";
	}

	/****************************** OPENGL HANDLING ******************************/

	// Load OpenGL
	GLFWwindow* window = Init_OpenGL();
	// Initialize GLUT, else we can't draw spheres and cones
	glutInit(&argc, argv);

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

	string LevelName = FindArgumentParameter(argc, argv, "-level");
	if (LevelName.size() == 0 || LevelName[0] == '-')
	{
		LevelName = "level.txt";
	}

	if (LevelName.length() > 0)
	{
		cout << "Loading level '" + LevelName << "'" << endl;
	}
	else
	{
		cin.ignore(cin.rdbuf()->in_avail());
		cout << "Enter a level's name: ";
		cin >> LevelName;
	}

	Level* CurrentLevel = F_LoadLevel(LevelName);
	if (!CurrentLevel)
	{
		cout << "FATAL ERROR: Cannot load level '" << LevelName << "'" << endl;
		exit(EXIT_FAILURE);
	}

	/****************************** SETUP PHASE ******************************/

	// Player
	Player* play = new Player;

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
			string line;
			//Demo Play is True
			if (!getline(DemoRead, line))
				Quit = true;
			if (!line.empty())
				play->Cmd.forward = stoi(line);
			if (!getline(DemoRead, line))
				Quit = true;
			if (!line.empty())
				play->Cmd.lateral = stoi(line);
			if (!getline(DemoRead, line))
				Quit = true;
			if (!line.empty())
				play->Cmd.rotation = stoi(line);
		}
		else
		{
			//Get Input from Keyboard
			// Input for tests
			if (!((glfwGetKey(window, GLFW_KEY_W) || glfwGetKey(window, GLFW_KEY_UP)) && (glfwGetKey(window, GLFW_KEY_S) || glfwGetKey(window, GLFW_KEY_DOWN))))
			{
				if (glfwGetKey(window, GLFW_KEY_W) || glfwGetKey(window, GLFW_KEY_UP))
				{
					play->Cmd.forward = 10;
				}
				if (glfwGetKey(window, GLFW_KEY_S) || glfwGetKey(window, GLFW_KEY_DOWN))
				{
					play->Cmd.forward = -10;
				}
			}
			if (!(glfwGetKey(window, GLFW_KEY_A) && glfwGetKey(window, GLFW_KEY_D)))
			{
				if (glfwGetKey(window, GLFW_KEY_A))
				{
					play->Cmd.lateral = -10;
				}
				if (glfwGetKey(window, GLFW_KEY_D))
				{
					play->Cmd.lateral = 10;
				}
			}
			if (!(glfwGetKey(window, GLFW_KEY_LEFT) && glfwGetKey(window, GLFW_KEY_RIGHT)))
			{
				if (glfwGetKey(window, GLFW_KEY_LEFT))
				{
					play->Cmd.rotation = 200;
				}
				if (glfwGetKey(window, GLFW_KEY_RIGHT))
				{
					play->Cmd.rotation = -200;
				}
			}

			if (DemoWrite.is_open())
			{
				DemoWrite << static_cast<int>(play->Cmd.forward) << endl;
				DemoWrite << static_cast<int>(play->Cmd.lateral) << endl;
				DemoWrite << static_cast<int>(play->Cmd.rotation) << endl;
			}
		}

		// Keys that don't affect the player
		if (glfwGetKey(window, GLFW_KEY_R))
		{
			play->Angle = 0;
			play->PosX = 0;
			play->PosY = 0;
			play->PosZ = 128;
			play->MoX = 0;
			play->MoY = 0;
			play->MoZ = 0;	
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE))
		{
			play->PosZ = play->PosZ + 10;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL))
		{
			play->PosZ = play->PosZ - 10;
		}
		if (glfwGetKey(window, GLFW_KEY_ESCAPE))
		{
			Quit = true;
		}

		//Send Commands over Network

		//Update game logic
		play->ExecuteTicCmd();

		//Draw Screen
		DrawScreen(window, play, CurrentLevel);

		//Play sound

		// Status of the player for debugging purposes
		if (Debug)
			cout << static_cast<int>(play->PosX) << '\n' << static_cast<int>(play->PosY) << '\n' << play->Angle << endl;

		TicCount++;

		//SDL_Delay(30);

		auto end = chrono::system_clock::now();
		auto diff = chrono::duration_cast<chrono::milliseconds>(end - start).count();
		if (TicCount % 5 == 0)
			SetWindowTitle(window, WindowTitle + " (" + to_string(1000 / diff) + "fps)");

		// Detect OpenGL errors
		GLenum ErrorCode;
		while ((ErrorCode = glGetError()) != GL_NO_ERROR)
		{
			cout << (const char*)gluErrorString(ErrorCode) << endl;
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
