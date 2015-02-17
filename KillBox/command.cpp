// command.cpp
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

// Command line parameters

#include <iostream>
#include <string>
#include <string.h>     //Used for strcmp()
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <time.h>

#include "command.h"

using namespace std;

//Generate random numbers to a file
void GenerateRandomNumbers(unsigned int Number, string Name)
{
	srand(static_cast<unsigned int>(time(NULL)));
	string FileName = Name + ".txt";
	ofstream myfile(FileName.c_str());

	if (myfile.is_open())
	{
		for (unsigned int i = 0; i < Number; i++)
		{
			myfile << rand() % 10;
			// cout << Number - i << endl;
		}

		cout << "Rand Nums -> Done!" << endl;
		myfile.close();
	}
	else
	{
		cout << "Unable to open file";
	}
}

//Return position of an argument
int FindArgumentPosition(int argc, char *argv[], string Argument)
{
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], Argument.c_str()) == 0)
		{
			return i;
		}
	}

	return 0;
}

//Print a list of arguments
void ListArguments(int argc, char *argv[])
{
	if (argc > 1)
	{
		cout << "Reading parameters..." << endl;

		int i = 1;
		bool None = true;

		while (i < argc)
		{
			if (argv[i][0] == '-')
			{
				cout << argv[i];
				None = false;
				i++;
				break;
			}

			i++;
		}

		while (i < argc)
		{
			if (argv[i][0] == '-')
			{
				cout << ", " << argv[i];
			}

			i++;
		}

		if (!None)
		{
			cout << endl;
		}
	}
}

void FatalError(string MyError)
{
	if (MyError.at(MyError.size() - 1) == '!')
	{
		cout << "FUCKSHIT: " << MyError << endl;
	}
	else
	{
		cout << "ERROR: " << MyError << endl;
	}

	// Wait until 'ENTER' is pressed, then exit. 
	WaitForEnterKey();
}

void WaitForEnterKey()
{
	// DOES NOT WORK AS INTENDED RIGHT NOW
	// SHOULD ONLY ACCEPT ENTER AND NOT PRINT OTHER CHARACTERS

	/* cin.ignore(cin.rdbuf()->in_avail() + 1); */
	cin.ignore();	// Ignore the newline from the stream
	if (cin.get() == '\n')	// Act like a 'system("PAUSE")'
	{
		// Abord program
		exit(EXIT_FAILURE);
	}
}


