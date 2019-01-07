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
// level.cpp
// Level loader. Supports a native format and the OBJ format.

#include "level.h"
#include "vecmath.h"	/* Float3, ComputeNormal */
#include "things.h"	/* Player, Plane */
#include "cache.h"	/* Cache */
#include "physics.h"	/* AdjustPlayerToFloor */
#include "random.h"	/* Rand() */
#include "strutils.h"

#include <vector>
#include <string>
#include <iostream>	/* cout */
#include <fstream>	/* ifstream */
#include <sstream>	/* istringstream */
#include <iterator>	/* istream_iterator */
#include <chrono>
using namespace std;

Level::Level(const string& level, float scaling, unsigned int numOfPlayers)
{
	auto start = chrono::system_clock::now();
	levelname_ = level;
	scaling_ = scaling;
	LoadLevel(level, numOfPlayers);
	auto end = chrono::system_clock::now();
	auto diff = chrono::duration_cast<chrono::milliseconds>(end - start).count();
	cout << "Level loading took " << diff << "ms." << endl;
}

Level::~Level()
{
	// Delete things from the map
	for (unsigned int i = 0; i < things.size(); i++)
	{
		delete things[i];
	}

	// Delete planes
	for (unsigned int i = 0; i < planes.size(); i++)
	{
		delete planes[i];
	}

	// Delete the cache instance and its contents
	Cache::Instance()->DestroyInstance();
}

void Level::Reload()
{
	// Delete planes from memory
	for (unsigned int i = 0; i < planes.size(); i++)
	{
		delete planes[i];
	}

	// Delete planes from level
	planes.clear();
	// Load level
	reloaded_ = true;
	LoadLevel(levelname_, players.size());
}

// Update game entities
void Level::UpdateThings()
{
	for (unsigned int i = 0; i < things.size(); i++)
	{
		bool keep = things[i]->Update();

		if (!keep)
		{
			delete things[i];
			things.erase(things.begin() + i);
		}
	}
}

void Level::SpawnPlayer(Player* play)
{
	play->Reset();

	if (spawns.size() != 0)
	{
		SpawnSpot spawn = spawns[Rand() % spawns.size()];
		play->pos_ = spawn.pos_;
		play->Angle = spawn.Angle;
	}
	else
	{
		// You would get a floating point exception if there were no check
		throw runtime_error("No spawn spot exists. Cannot spawn player.");
	}
}

void Level::AddTexture(const string& name, bool enableFiltering)
{
	if (Cache::Instance()->Add(name, enableFiltering))
	{
		cout << "Added texture " << name << endl;
	}
}

void Level::UseTexture(const string& name)
{
	// Avoid rebinding the texture if it's already binded
	if (name != lastTextureBind)
	{
		lastTextureBind = name;
		Cache::Instance()->Get(name)->Bind();
	}
}

// Detects the format and calls the right loading method
void Level::LoadLevel(const string& LevelName, unsigned int numOfPlayers)
{
	if (EndsWith(LevelName, ".obj"))
	{
		LoadObj(LevelName, numOfPlayers);
		useUVs_ = true;
	}
	else
	{
		LoadNative(LevelName, numOfPlayers);
	}
}

bool Level::HasUVs() const
{
	return useUVs_;
}

// Loading method for native format
void Level::LoadNative(const string& LevelName, unsigned int numOfPlayers)
{
	string FileName = LevelName;       // Added to make some compiler happy
	ifstream LevelFile(FileName.c_str());
	if (LevelFile.is_open())
	{
		bool blurTextures = false;
		unsigned int Count = 0;
		string Line;
		while (!LevelFile.eof())     // Read the entire file until the end
		{
			getline(LevelFile, Line);
			Count++;

			if (Line.size() > 0 && Line[0] != '#')
			{
				//vector<string> tokens = Split(Line, '\t');
				istringstream buf(Line);
				istream_iterator<string> beg(buf), end;
				vector<string> tokens(beg, end);

				if (tokens[0] == "thing" && tokens.size() == 6)
				{
					if (tokens[1] == "spawn")
					{
						SpawnSpot spawn;
						spawn.pos_.x = atof(tokens[2].c_str());
						spawn.pos_.y = atof(tokens[3].c_str());
						spawn.pos_.z = atof(tokens[4].c_str());
						spawn.Angle = (short)atoi(tokens[5].c_str()) * 91.0222222222f;
						spawns.push_back(spawn);
					}
					else if (tokens[1] == "player")
					{
						play = new Player();
						play->pos_.x = atof(tokens[2].c_str());
						play->pos_.y = atof(tokens[3].c_str());
						play->pos_.z = atof(tokens[4].c_str());
						play->Angle = (short)atoi(tokens[5].c_str()) * 91.0222222222f;
						players.push_back(play);
					}
					else if (tokens[1] == "weapon")
					{
						weapons.push_back(new Weapon(atof(tokens[2].c_str()), atof(tokens[3].c_str()), atof(tokens[4].c_str()), tokens[5]));
					}
				}
				else if (tokens[0] == "poly" && (tokens.size() == 21 || tokens.size() == 18))
				{
					Plane* p = new Plane();
					p->Impassable = tokens[2][0] != '0';
					p->TwoSided = tokens[3][0] != '0';
					p->Xscale = atof(tokens[4].c_str());
					p->Yscale = atof(tokens[5].c_str());
					p->Light = atof(tokens[8].c_str());

					// polygons are quads or triangles
					for (unsigned int i = 9; i < tokens.size(); i += 3)
					{
						Float3 vt;
						vt.x = atof(tokens[i].c_str());
						vt.y = atof(tokens[i+1].c_str());
						vt.z = atof(tokens[i+2].c_str());
						p->Vertices.push_back(vt);
					}

					if (tokens[1] != "INVISIBLE")
					{
						AddTexture(tokens[1], blurTextures);	// Add texture to cache
						p->Texture = tokens[1];
					}

					p->Process();
					planes.push_back(p);
				}
				else if (tokens[0] == "setting" && tokens.size() == 3)
				{
					if (tokens[1] == "blur")
					{
						blurTextures = tokens[2][0] != '0';
					}
					else if (tokens[1] == "skytex")
					{
						AddTexture(tokens[2], blurTextures);
						SkyTexture = tokens[2];
					}
					else if (tokens[1] == "skyele")
					{
						SkyHeigth = atof(tokens[2].c_str());
					}
				}
				else
				{
					cout << "Skipped line " << Count << endl;
				}
			}
		}

		cout << "Read " << Count - 1 << " lines from file. " << endl;
		LevelFile.close();

		BuildBlockmap();

		// Check if no player was created
		if (players.size() == 0)
		{
			// Create the required number of players and spawn them
			for (unsigned int i = 0; i < numOfPlayers; i++)
			{
				players.emplace_back(new Player());
				SpawnPlayer(players[i]);
			}
			// Set the player to player #1
			play = players[0];
		}

		// Populate the array of "things"
		things.insert(things.end(), weapons.begin(), weapons.end());
		things.insert(things.end(), players.begin(), players.end());
	}
	else
	{
		cout << "Unable to open level file!" << endl;
	}
}

// Loading method for OBJ format
void Level::LoadObj(const string& path, unsigned int numOfPlayers)
{
	cout << "Loading 3D model: " << path << endl;

	ifstream model;
	model.open(path);

	// Create temporary variables
	vector<Float3> temp_vertices;
	vector<Float2> temp_uvs;
	vector<Float3> temp_normals;

	string texture = "None";	// Current texture for plane
	SkyTexture = "clouds.jpg";
	AddTexture(SkyTexture, true);

	if (model.is_open())
	{
		unsigned int Count = 0;
		string Line;

		while (!model.eof())
		{
			getline(model, Line);
			Count++;

			// Do something with that line
			if (Line.size() > 0 && Line[0] != '#')
			{
				vector<string> slices = Split(Line, ' ');

				if (slices[0] == "v" && slices.size() == 4)	// Vertex
				{
					Float3 temp_vertex;
					// Puts Z X Y into X Y Z because most OBJ exporters use this format
					temp_vertex.x = atof(slices[3].c_str()) * scaling_;
					temp_vertex.y = atof(slices[1].c_str()) * scaling_;
					temp_vertex.z = atof(slices[2].c_str()) * scaling_;
					temp_vertices.push_back(temp_vertex);
				}
				else if (slices[0] == "vt" && slices.size() == 3)	// Texture coordinate of a vertex
				{
					Float2 temp_uv;
					temp_uv.x = atof(slices[1].c_str());
					temp_uv.y = atof(slices[2].c_str());
					temp_uvs.push_back(temp_uv);
				}
				else if (slices[0] == "vn" && slices.size() == 4)	// Normal of a vertex
				{
					Float3 temp_normal;
					temp_normal.x = atof(slices[1].c_str());
					temp_normal.y = atof(slices[2].c_str());
					temp_normal.z = atof(slices[3].c_str());
					temp_normals.push_back(temp_normal);
				}
				else if (slices[0] == "f" && (slices.size() == 4 || slices.size() == 5))	// Defines a face
				{
					// Create a plane for a set of vertices
					Plane* p = new Plane();
					p->Impassable = 1;
					p->TwoSided = 0;
					p->Xscale = 1;
					p->Yscale = 1;
					p->Light = 1;

					// Assign the last specified texture if the plane is not invisible
					if (texture != "None")
					{
						p->Texture = texture;
					}

					// Format: vertex, uv, normal. They are indices that points to the previous data.
					for (unsigned int i = 1; i < slices.size(); i++)
					{
						vector<string> indices = Split(slices[i], '/');

						if (indices.size() >= 1)
						{
							vertices_.push_back(temp_vertices[atoi(indices[0].c_str())-1]);
							p->Vertices.push_back(temp_vertices[atoi(indices[0].c_str())-1]);
						}

						if (indices.size() >= 2)
						{
							uvs_.push_back(temp_uvs[atoi(indices[1].c_str())-1]);

							if (indices.size() == 3)
							{
								normals_.push_back(temp_normals[atoi(indices[2].c_str())-1]);
							}
						}
					}

					p->Process();
					planes.push_back(p);
				}
				else if (slices[0] == "usemtl" && slices.size() == 2)
				{
					texture = slices[1];
					string folder = "";

					// Get the directory of the OBJ file because the textures should be in the same directory
					texture = DirName(path) + texture;

					if (!EndsWith(texture, "None"))
					{
						AddTexture(texture, false);
					}
					else	// Texture is "None"
					{
						texture = "None";
					}
				}
				else if (slices[0] == "thing" && slices.size() == 6)
				{
					if (slices[1] == "spawn")
					{
						SpawnSpot spawn;
						spawn.pos_.x = atof(slices[2].c_str()) * scaling_;
						spawn.pos_.y = atof(slices[3].c_str()) * scaling_;
						spawn.pos_.z = atof(slices[4].c_str()) * scaling_;
						spawn.Angle = (short)atoi(slices[5].c_str()) * 91.0222222222f;
						spawns.push_back(spawn);
					}
					else if (slices[1] == "player")
					{
						play = new Player();
						play->pos_.x = atof(slices[2].c_str()) * scaling_;
						play->pos_.y = atof(slices[3].c_str()) * scaling_;
						play->pos_.z = atof(slices[4].c_str()) * scaling_;
						play->Angle = (short)atoi(slices[5].c_str()) * 91.0222222222f;
						players.push_back(play);
					}
					else if (slices[1] == "weapon")
					{
						weapons.push_back(new Weapon(atof(slices[2].c_str()) * scaling_, atof(slices[3].c_str()) * scaling_, atof(slices[4].c_str()) * scaling_, slices[5]));
					}
				}
				else if (slices[0] == "setting" && slices.size() == 3)
				{
					if (slices[1] == "scale")
					{
						scaling_ = atof(slices[2].c_str());
					}
				}
				else
				{
					cout << "Skipped line " << Count << endl;
				}
			}
		} // end of while loop

		BuildBlockmap();

		// Check if no player was created
		if (players.size() == 0)
		{
			// Create the required number of players and spawn them
			for (unsigned int i = 0; i < numOfPlayers; i++)
			{
				players.emplace_back(new Player());
				SpawnPlayer(players[i]);
				AdjustPlayerToFloor(players[i], this);
			}
			// Set the player to player #1
			play = players[0];
		}

		// Populate the array of "things"
		things.insert(things.end(), weapons.begin(), weapons.end());
		things.insert(things.end(), players.begin(), players.end());

		if (uvs_.empty())
		{
			// This should help diagnostics
			cerr << "WARNING: No UVs found. The program is expected to crash." << endl;
		}

		// Set UVs
		unsigned int uv_count = 0;
		for (unsigned int i = 0; i < planes.size(); i++)
		{
			for (unsigned int j = 0; j < planes[i]->Vertices.size(); j++)
			{
				// Distribute an UV to each vertice
				planes[i]->UVs.push_back(uvs_[uv_count]);

				uv_count++;
			}
		}

		// Remove polygons that don't have a texture
		for (int i = planes.size() - 1; i >= 0; i--)
		{
			if (planes[i]->Texture == "")
			{
				delete planes[i];
				planes.erase(planes.begin() + i);
			}
		}
	}

	model.close();
}

void Level::BuildBlockmap()
{
	// TODO: The blockmap will be used as an optimization
}
