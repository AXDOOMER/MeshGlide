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
// random.cpp
// Pseudo-random number generator (PRNG)

#include "random.h"

// How it works: R_Random( % , * , + )
// Default values should be ( 256, 1, 0) for the wider range of results
int R_Random()
{
	Index = Index % 256;
	return PRNG[Index++];
}

void R_Reset()
{
	Index = 0;
}
