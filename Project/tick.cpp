// Copyright (C) 2017-2019 Alexandre-Xavier Labonté-Lamoureux
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
// tick.cpp
// Single instance of a game action sampled per tick for a player

#include "tick.h"

#include <string>

using namespace std;

TicCmd::TicCmd()
{
	Reset();
	quit = false;
}

void TicCmd::Reset()
{
	// Init to default values
	forward = 0;
	lateral = 0;
	rotation = 0;
	vertical = 0;
	fire = false;
	chat = "";
}
