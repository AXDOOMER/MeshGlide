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
#include "vecmath.h"	/* Float3 */
#include "things.h"	/* Player, Plane */
#include "cache.h"	/* Cache */
#include "physics.h" /* AdjustPlayerToFloor */

#include <vector>
#include <string>
#include <iostream>	/* cout */
#include <fstream>	/* ifstream */
#include <sstream>	/* istringstream */
#include <iterator>	/* istream_iterator */
#include <chrono>
using namespace std;

Level::Level(const string& level, float scaling)
{
	auto start = chrono::system_clock::now();
	LoadLevel(level, scaling);
	auto end = chrono::system_clock::now();
	auto diff = chrono::duration_cast<chrono::milliseconds>(end - start).count();
	cout << "Level loading took " << diff << "ms." << endl;
}

Level::~Level()
{
	if (play != nullptr)
	{
		delete play;
	}

	// Delete planes
	for (unsigned int i = 0; i < planes.size(); i++)
	{
		delete planes[i];
	}
}

void Level::AddTexture(const string& name, bool enableFiltering)
{
	if (!cache.Has(name))
	{
		cache.Add(name, new Texture(name, enableFiltering));
		cout << "Added texture " << name << endl;
	}
}

void Level::UseTexture(const string& name)
{
	cache.Get(name)->Bind();
}

// Splits a string
vector<string> Level::Split(string s, const string& delimiter)
{
	size_t pos = 0;
	vector<string> tokens;
	string token;
	while ((pos = s.find(delimiter)) != string::npos) {
		token = s.substr(0, pos);
		tokens.push_back(token);	// add to vector
		s.erase(0, pos + delimiter.length());
	}

	// There may not be any delimiter remaining, but let's not forget the last token.
	tokens.push_back(s);

	return tokens;
}

bool Level::EndsWith(const string& str, const string& value)
{
	return str.rfind(value) == str.size() - value.size();
}

// Detects the format and calls the right loading method
void Level::LoadLevel(const string& LevelName, float scaling)
{
	if (EndsWith(LevelName, ".obj"))
	{
		LoadObj(LevelName, scaling);
		AdjustPlayerToFloor(play, this);
	}
	else
	{
		LoadNative(LevelName);
	}
}

// Loading method for native format
void Level::LoadNative(const string& LevelName)
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
				//vector<string> tokens = Split(Line, "\t");
				istringstream buf(Line);
				istream_iterator<string> beg(buf), end;
				vector<string> tokens(beg, end);

				if (tokens[0] == "thing" && tokens.size() == 6)
				{
					if (tokens[1] == "player")
					{
						play = new Player;
						play->pos_.x = atof(tokens[2].c_str());
						play->pos_.y = atof(tokens[3].c_str());
						play->pos_.z = atof(tokens[4].c_str());
						play->Angle = (short)atoi(tokens[5].c_str()) * 91.0222222222f;
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

					p->ComputeWallInfo();			// Comment out this line to become a ghost
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
					cout << "Skipped line: " << Count << endl;
				}
			}
		}

		cout << "Read " << Count << " lines from file. " << endl;
		LevelFile.close();

		LinkPlanes(LevelName);
	}
	else
	{
		cout << "Unable to open level file!" << endl;
	}
}

// Loading method for OBJ format
void Level::LoadObj(const string& path, float scaling)
{
	cout << "Loading 3D model: " << path << endl;

	ifstream model;
	model.open(path);

	// Create temporary variables
	vector<Float3> temp_vertices;
	vector<Float2> temp_uvs;
	vector<Float3> temp_normals;

	string texture = "grass.png";	// Current texture for plane

	if (model.is_open())
	{
		unsigned int Count = 0;
		string Line;

		// Create player for level
		play = new Player;
		play->pos_ = {0, 0, 0};
		play->Angle = 0;

		while (!model.eof())
		{
			getline(model, Line);
			Count++;

			// Do something with that line
			if (Line.size() > 0 && Line[0] != '#')
			{
				vector<string> slices = Split(Line, " ");

				if (slices[0] == "v" && slices.size() == 4)	// Vertex
				{
					Float3 temp_vertex;
					temp_vertex.x = atof(slices[1].c_str()) * scaling;
					temp_vertex.y = atof(slices[2].c_str()) * scaling;
					temp_vertex.z = atof(slices[3].c_str()) * scaling;
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

					// Use a default texture because materials and UVs are not implemented
					AddTexture(texture, false);
					p->Texture = texture;

					p->ComputeWallInfo();	// For walls

					// Format: vertex, uv, normal. They are indices that points to the previous data.
					for (unsigned int i = 1; i < slices.size(); i++)
					{
						vector<string> indices = Split(slices[i], "/");

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

					planes.push_back(p);
				}
				else if (slices[0] == "usemtl" && slices.size() == 2)
				{
					texture = slices[1];
				}
				else
				{
					cout << "Skipped line: " << Count << endl;
				}
			}
		} // end of while loop

		LinkPlanes(path);
	}

	model.close();
}

void Level::LinkPlanes(const string& LevelName)
{
	ifstream ReadLinks;
	ofstream WriteLinks;

	ReadLinks.open(LevelName + ".lnb");

	if (ReadLinks.is_open())
	{
		cout << "Found linked planes data file. Loading..." << endl;

		string Line;
		// Read the entire file until the end
		while (!ReadLinks.eof())
		{
			getline(ReadLinks, Line);

			vector<string> indices = Split(Line, " ");

			for (unsigned int i = 1; i < indices.size(); i++)
			{
				planes[atoi(indices[0].c_str())]->Neighbors.push_back(planes[atoi(indices[i].c_str())]);
			}
		}
	}
	else
	{
		WriteLinks.open(LevelName + ".lnb");

		if (WriteLinks.is_open())
		{
			cout << "Linked planes data file not found. Generating..." << endl;

			for (unsigned int i = 0; i < planes.size(); i++)
			{
				bool first = true;

				// Find touching planes and add them
				for (unsigned int j = 0; j < planes.size(); j++)
				{
					if (i != j)
					{
						// Find more than one common vertex
						if (planes[i]->CommonVertices(planes[j]) > 1)
						{
							if (first)
							{
								WriteLinks << i;
								first = false;
							}

							planes[i]->Neighbors.push_back(planes[j]);

							WriteLinks << " " << j;
						}
					}
				}

				if (!first)
				{
					WriteLinks << endl;
				}
			}
		}
	}

	ReadLinks.close();
	WriteLinks.close();
}
