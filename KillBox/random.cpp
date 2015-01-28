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

int R_Random(int Modulo, int Spacing, int Modifier)
{
	if (Modulo < 1)
	{
		Modulo = PRA_SIZE;
	}

	if (Spacing < 1)
	{
		Spacing = 1;
	}

	int Number = static_cast<int>(RandomNumbers[Index]);

	Index++;

	if (Index == PRA_SIZE)
	{
		Index = 0;
	}

	if (Index > PRA_SIZE)
	{
		cout << "Pseudo-Random Number Array Overflow! Index is " << Index << endl;
	}

	return (Number % Modulo) * Spacing + Modifier;
}
