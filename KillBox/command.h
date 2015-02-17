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

// Command line parameter


#include <string>
using namespace std;

#pragma once

// Generate random numbers to a file
void GenerateRandomNumbers(unsigned int Number, string Name);

// Return position of an argument
int FindArgumentPosition(int argc, char *argv[], string Argument);

// Print a list of arguments
void ListArguments(int argc, char *argv[]);

// Print a message about an error. Will cause the program to stop. 
void FatalError(string MyError);

// Wait for enter key 
void WaitForEnterKey();

