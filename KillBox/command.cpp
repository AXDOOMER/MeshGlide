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
// command.cpp
// Command line parameters

#include <iostream>
#include <string>
#include <string.h>     //Used for strcmp()
#include <sstream>
#include <fstream>

#include "command.h"

using namespace std;

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

string FindArgumentParameter(int argc, char *argv[], string Argument)
{
	int argpos = FindArgumentPosition(argc, argv, Argument);
	if (argpos > 0 && argpos + 1 < argc)
	{
		return argv[argpos + 1];
	}
	return "";
}
