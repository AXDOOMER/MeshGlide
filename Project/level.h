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

#include "actor.h"
#include "player.h"
#include "plane.h"	/* Plane */
#include "cache.h"	/* Cache */
#include "vecmath.h"	/* Float3 */

#include <vector>
#include <string>
using namespace std;

class Level
{
public:
	float SkyHeigth = 5.0f;	// Sky elevation
	string SkyTexture;

	// Stuff that's part of the map
	vector<Plane*> planes;
	Player* play = nullptr;	// Pointer to the current player
	vector<Player*> players;	// Pointers to every player
	vector<SpawnSpot> spawns;
	vector<Weapon*> weapons;
	vector<Actor*> things;	// In order to draw everything easily, everything is put in the same array. TODO: Use a deque?

	void AddTexture(const string& name, bool enableFiltering);	// Add texture to cache if missing
	void UseTexture(const string& name);	// Bind texture

	Level(const string& level, float scaling, unsigned int numOfPlayers);
	~Level();
	void Reload();	// Reload level geometry

	void LoadLevel(const string& LevelName, unsigned int numOfPlayers);
	void LoadNative(const string& LevelName, unsigned int numOfPlayers);
	void LoadObj(const string& path, unsigned int numOfPlayers);

	void SpawnPlayer(Player* play, const vector<Player*>& players);
	void UpdateThings();

	bool HasUVs() const;

	vector<Plane*> getPlanesForBox(float x, float y, float radius) const;

private:
	// OBJ and OpenGL stuff
	vector<Float3> vertices_;
	vector<Float2> uvs_;
	vector<Float3> normals_;

	float scaling_ = 1.0f;	// Level scaling that adjusts the size of the level proportionally
	string levelname_;
	string lastTextureBind = "";
	bool reloaded_ = false;
	bool useUVs_ = false;

	void LinkPlanes(const string& LevelName);
	void FinalPlaneProcessing();
	void CountCommonEdgesPlanes(Plane* p1, Plane* p2);
};

#endif // LEVEL_H
