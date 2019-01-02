// Copyright (C) 2019 Alexandre-Xavier Labonté-Lamoureux
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
// defaults.h
// Default settings

#include <string>
using namespace std;

// Using a constant will result in a config name
const string FULLSCREEN = "fullscreen";
const string WINDOW_WIDTH = "window_width";
const string WINDOW_HEIGHT = "window_height";
const string POOL_EVENTS_TWICE = "pool_events_twice";
const string WINDOW_XPOS = "window_xpos";
const string WINDOW_YPOS = "window_ypos";
const string MOUSE_LOOK = "mouse_look";

const string EMPTY = "";

string disp[][2] = {
	{FULLSCREEN, "0"},
	{WINDOW_WIDTH, "640"},
	{WINDOW_HEIGHT, "480"},
	{POOL_EVENTS_TWICE, "1"},
	{WINDOW_XPOS, "-1"},
	{WINDOW_YPOS, "-1"},
	{MOUSE_LOOK, "1"},
	{EMPTY, EMPTY}
};

string GetDefault(const string& name)
{
	int i = 0;
	while (disp[i][0] != EMPTY)
	{
		if (disp[i][0] == name)
			return disp[i][1];

		i++;
	}

	return EMPTY;
}
