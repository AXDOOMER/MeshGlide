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

#include <ctime>
#include "random.h"

// Defined here in order to avoid "warning: 'Index' defined but not used"
static unsigned short Index_ = 0;

// Return the next "random" number
int Rand()
{
	Index_ = Index_ % NUMBERS;
	return PRNG[Index_++];
}

void ResetRand()
{
	Index_ = 0;
}

unsigned short GetIndex()
{
	return Index_;
}

void SetIndex(unsigned short Index)
{
	Index_ = Index;
}

// The seed will be different each seconds
void Seed()
{
	time_t rawtime;
	time (&rawtime);

	Index_ = rawtime /*% NUMBERS*/;
}
