// random.cpp
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

// Pseudo-random number generator (PRNG)

#include <iostream>
using namespace std;

#include "random.h"

//int Random()
//{
//	//TO-DO: Ajouter de quoi pour faire sûr qu'on remet l'index à 0 quand on arrive à la fin
//	int Number = /*static_cast<int>(*/RandomNumbers[index]/*)*/;
//	index++;
//	return Number;
//}
//
//int RandomModulo(int Modulo)
//{
//	int Number = static_cast<int>(RandomNumbers[index]);
//	index++;
//	return (Number % Modulo);
//}
//
//int RandomModuloSpacing(int Modulo, int Spacing)
//{
//	int Number = static_cast<int>(RandomNumbers[index]);
//	index++;
//	return (Number % Modulo) * Spacing;
//}

// How it works: R_Random( % , * , + )
// Default values should be ( 256, 1, 0) for the wider range of results
int R_Random(int Modulo, int Spacing, int Modifier)
{
	// Avoid a crash
	if (Modulo < 1)
	{
		// Make it bigger than the largest number so it doesn't have an impact
		Modulo = NUMBERS;
	}

	// It's useless to multiply a random number by zero
	if (Spacing < 1)
	{
		// Multiply it by one so it doesn't have an impact
		Spacing = 1;
	}

	// Get a number an switch the index to the next
	int Number = static_cast<int>(RandomNumbers[Index++]);

	// Do this so we don't read outside of the array
	if (Index == NUMBERS)
	{
		Index = 0;
	}

	// Should never happen unless there is a programming error
	if (Index > NUMBERS)
	{
		cout << "Pseudo-Random Number Array Overflow! Index is " << Index << endl;
	}

	// Do the operations that we want on the number then return it
	return (Number % Modulo) * Spacing + Modifier;
}
