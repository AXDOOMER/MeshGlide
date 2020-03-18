// Copyright (C) 2017-2020 Alexandre-Xavier Labonté-Lamoureux
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
// ticcmd.h
// Single instance of a game action sampled per tic for a player
// Sometimes called a "tic command"

#ifndef TICCMD_H
#define TICCMD_H

#include <string>
#include <vector>

using namespace std;

class Ticcmd
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

	Ticcmd();
	void Reset();

	vector<unsigned char> Serialize() const;	// raw binary
	vector<unsigned char> Serialize2() const;	// line of text
	void Deserialize(vector<unsigned char> v);	// raw binary
	void Deserialize2(vector<unsigned char> v);	// line of text
};

#endif /* TICCMD_H */
