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
// tick.h
// Single instance of a game action sampled per tick for a player

#ifndef TICK_H
#define TICK_H

#include <string>

using namespace std;

class TicCmd
{
public:
	TicCmd();
	void Reset();

	unsigned char id;
	signed char forward;
	signed char lateral;
	short rotation;
	short vertical;
	bool fire;
	bool quit;
	string chat;
};

#endif /* TICK_H */
