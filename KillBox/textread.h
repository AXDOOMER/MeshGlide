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
// textread.h
// Handles text data reading from file & writing
// Used for config files, levels and demo files.


#include <string>

#include "things.h"
using namespace std;

//Load le niveau et retourne son pointeur
Level* F_LoadLevel(string LevelName);

//Loader une démo pour se servir de son input
//int F_LoadDemo(string DemoName);
