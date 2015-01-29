// main.cpp
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

// The main stuff is here

// These instructions tell where to put them:
// http://stackoverflow.com/questions/22539047/how-can-i-set-up-freeglut-and-glew-for-use-in-visual-studio-2013
#include <GLFW/glfw3.h>
#include <GL/freeglut.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include <iostream>
#include <string>
#include <string.h>     //Used for strcmp()
#include <stdlib.h>     /* atoi */

#include "viewdraw.h"
#include "command.h"
#include "random.h"
#include "things.h"
#include "textread.h"

using namespace std;

//Global variables
int Global = 0xCAFEBABE;

int main(int argc, char *argv[])
{
	// Creating the first memory pointer
	int *ptr = new int;
	const int VERSION = 0;		// A serial number for the version

	bool Quit = false;
	static unsigned int TicCount = 0;

	string ProgramName = argv[0];
	ProgramName = ProgramName.substr(ProgramName.find_last_of("/\\"), ProgramName.length());
	ProgramName[0] = ' ';	// The first character here will be a space

	cout << "                KILLBOX DEV VERSION " <<		// Should be named "KillBox"
		VERSION / 100 << "." << VERSION % 100 << " (PRE-ALPHA)" << endl;
	cout << "Memory area: Is around 0x" << &ptr << endl;       //approx area ~
	cout << "Program name:" << ProgramName << endl << endl;

	// Remove the first pointer from memory
	delete ptr;
	ptr = 0;

	ListArguments(argc, argv);

	cout << "                            WARNING" << endl;
	cout << "================================================================" << endl;
	cout << "  \"KillBox\" is free software, covered by the GNU General Public" << endl;
	cout << " License. There is NO warranty and the author is NOT liable for" << endl;
	cout << " any results of its use. Read the license BEFORE you continue." << endl;
	cout << "                    PRESS A KEY TO CONTINUE" << endl;
	cout << "================================================================" << endl;
	cin.get();

	if (FindArgumentPosition(argc, argv, "-debug") > 0)
	{
		cout << "I_sys: Debug mode ON." << endl;
	}

	if (FindArgumentPosition(argc, argv, "-file") > 0 && argv[(FindArgumentPosition(argc, argv, "-file") + 1)] > 0)
	{
		cout << "WADunit_ " << argv[(FindArgumentPosition(argc, argv, "-file") + 1)] << endl;
	}

	string DemoPlay = "";
	if (argv[FindArgumentPosition(argc, argv, "-playdemo") + 1] > 0)
	{
		DemoPlay = argv[FindArgumentPosition(argc, argv, "-playdemo") + 1];
		if (DemoPlay[0] != '-')
		{
			DemoPlay = argv[FindArgumentPosition(argc, argv, "-playdemo") + 1];
		}
	}

	if (FindArgumentPosition(argc, argv, "-random") > 0)
	{
		if (argv[FindArgumentPosition(argc, argv, "-random") + 1] > 0 &&
			argv[(FindArgumentPosition(argc, argv, "-random") + 2)] > 0)
		{
			string Number = argv[FindArgumentPosition(argc, argv, "-random") + 1];

			if (Number[0] != '-')
			{
				string FileName = argv[FindArgumentPosition(argc, argv, "-random") + 2];
				unsigned int Lenght = atoi(Number.c_str());

				if (FileName[0] != '-')
				{
					GenerateRandomNumbers(Lenght, FileName);
				}
				else
				{
					cout << "ERROR with parameter '-random'. File name is missing!" << endl;
					cout << "Type output file name: ";
					cin >> FileName;
					GenerateRandomNumbers(Lenght, FileName);
				}
			}
			else
			{
				cout << "ERROR with parameter '-random'. Check arguments!" << endl;
			}
		}
		else
		{
			cout << "ERROR with parameter '-random'. Missing arguments!" << endl;
		}
	}

	cout << "Enter a level's name: ";
	string LevelName;
	cin >> LevelName;
	Level* CurrentLevel = F_LoadLevel(LevelName);

	// Load somem stuff
	cout << "Graphics: Loading [";
	for (int i = 0; i < 20; i++)
	{
		cout << ".";
	}
	cout << "]" << endl;

	// Load OpenGL
	GLFWwindow* window = Init_OpenGL();
	if (!window)
	{
		cout << "An error as occurend when tried to initialise OpenGL stuff!" << endl;
		exit(EXIT_FAILURE);
	}

	if (FindArgumentPosition(argc, argv, "-wireframe") > 0)
	{
		cout << "_OpenGL: Wireframe mode activated." << endl;
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	// Temporary player
	Player* play = new Player;


    //Game loop
    do
    {
		if (DemoPlay.size() > 0)
		{
			//Demo Play is True
		}
		else
		{
			//Get Input from Keyboard
		}

		//Send Commands over Network

		//Update game logic
		
		//Draw Screen

		//Play sound

		// Input for tests
		if (glfwGetKey(window, GLFW_KEY_W))
		{
			play->ForwardMove(5);
		}
		if (glfwGetKey(window, GLFW_KEY_S))
		{
			play->ForwardMove(-5);
		}
		if (glfwGetKey(window, GLFW_KEY_A))
		{
			play->LateralMove(-5);
		}
		if (glfwGetKey(window, GLFW_KEY_D))
		{
			play->LateralMove(5);
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT))
		{
			play->AngleTurn(1);
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT))
		{
			play->AngleTurn(-1);
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE))
		{
			play->Angle = 0;
			play->PosX = 0;
			play->PosY = 0;
			play->PosZ = 0;
			play->MoX = 0;
			play->MoY = 0;
			play->MoZ = 0;	
		}

		// Status of the player for debugging purposes
		cout << static_cast<int>(play->PosX) << "		" << static_cast<int>(play->PosY) << "		" << play->Angle << endl;


		glfwSwapBuffers(window);
		glfwPollEvents();

        TicCount++;
    }
	while (!Quit && !glfwWindowShouldClose(window));

	cout << endl << endl << "Press any key to terminate the program." << endl;
	cin.get();

	// Close OpenGL stuff
	Close_OpenGL();

	return 0;
}
