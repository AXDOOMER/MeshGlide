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
// Sometimes called a "tic command"

#ifndef TICK_H
#define TICK_H

#include <string>
#include <vector>

using namespace std;

class Tick
{
public:
	unsigned char id;
	signed char forward;
	signed char lateral;
	short rotation;
	short vertical;
	bool fire;
	bool quit;
	string chat;

	Tick();
	void Reset();

	vector<unsigned char> Serialize() const;
	void Deserialize(vector<unsigned char> v);
};

#endif /* TICK_H */
