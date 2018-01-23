// Copyright (C) 2014-2018 Alexandre-Xavier Labonté-Lamoureux
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
// level.h
// Level loader. Supports a native format and the OBJ format.

#ifndef LEVEL_H
#define LEVEL_H

#include "vecmath.h"	/* Float3 */
#include "things.h"	/* Player, Plane */
#include "cache.h"	/* Cache */

#include <vector>
#include <string>
using namespace std;

class Level
{
public:
	Cache<string, Texture> cache;
	vector<Plane*> planes;
	Player* play = nullptr;
	float scaling = 1.0f;	// Level scaling
	float SkyHeigth = 5.0f;	// Sky elevation
	string SkyTexture;

	void AddTexture(const string& name, bool enableFiltering);	// Add texture to cache if missing
	void UseTexture(const string& name);	// Bind texture

	Level(const string& level, float scaling);
	~Level();

	vector<string> Split(string s, const string& delimiter);
	bool EndsWith(const string& str, const string& value);
	void LoadLevel(const string& LevelName, float scaling);
	void LoadNative(const string& LevelName);
	void LoadObj(const string& path, float scaling);

private:
	// OBJ and OpenGL stuff
	vector<Float3> vertices_;
	vector<Float2> uvs_;
	vector<Float3> normals_;

	void LinkPlanes(const string& LevelName);
};

#endif // LEVEL_H
