// Copyright (C) 2017-2018 Alexandre-Xavier Labonté-Lamoureux
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
// objload.h
// Wavefront OBJ format mesh loader

#ifndef _OBJLOAD_H
#define _OBJLOAD_H

#include "vecmath.h"	/* Float3 */
#include "things.h"	/* Level */

#include <vector>
#include <string>
#include <iostream>	/* cout */
#include <fstream>

using namespace std;

class Obj
{
public:
	vector<Float3> vertices_;
	vector<Float2> uvs_;
	vector<Float3> normals_;

	Level* lvl;

	// Splits a string
	vector<string> Split(string s, const string& delimiter)
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

	Obj(const string& path)
	{
		cout << "Loading 3D model: " << path << endl;

		ifstream model;
		model.open(path);

		// Create temporary variables
		vector<Float3> temp_vertices;
		vector<Float2> temp_uvs;
		vector<Float3> temp_normals;

		if (model.is_open())
		{
			unsigned int Count = 0;
			string Line;

			lvl = new Level();

			// Create player for level
			lvl->play = new Player;
			lvl->play->pos_ = {0, 0, 50};	// Height is intentionally set high
			lvl->play->Angle = 0;

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
						temp_vertex.x = atof(slices[1].c_str());
						temp_vertex.y = atof(slices[2].c_str());
						temp_vertex.z = atof(slices[3].c_str());
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
					else if (slices[0] == "f" && slices.size() == 4)	// Defines a face
					{
						// Create a plane for a set of vertices
						Plane* p = new Plane();
						p->Impassable = 1;
						p->TwoSided = 0;
						p->Xscale = 1;
						p->Yscale = 1;
						p->Light = 1;

						// Use a default texture because materials and UVs are not implemented
						lvl->AddTexture("grass.png", false);
						p->Texture = "grass.png";

						p->ComputeWallInfo();	// For walls

						// Format: vertex, uv, normal. They are indices that points to the previous data.
						vector<string> indices = Split(slices[1], "/");

						vertices_.push_back(temp_vertices[atoi(indices[0].c_str())-1]);
						p->Vertices.push_back(temp_vertices[atoi(indices[0].c_str())-1]);
//						uvs_.push_back(temp_uvs[atoi(indices[1].c_str())-1]);
						normals_.push_back( temp_normals[atoi( indices[2].c_str())-1]);

						indices = Split(slices[2], "/");

						vertices_.push_back(temp_vertices[atoi(indices[0].c_str())-1]);
						p->Vertices.push_back(temp_vertices[atoi(indices[0].c_str())-1]);
//						uvs_.push_back(temp_uvs[atoi(indices[1].c_str())- 1]);
						normals_.push_back(temp_normals[atoi(indices[2].c_str())-1]);

						indices = Split(slices[3], "/");

						vertices_.push_back(temp_vertices[atoi(indices[0].c_str())-1]);
						p->Vertices.push_back(temp_vertices[atoi(indices[0].c_str())-1]);
//						uvs_.push_back(temp_uvs[atoi(indices[1].c_str())-1]);
						normals_.push_back(temp_normals[atoi(indices[2].c_str())-1]);

						lvl->planes.push_back(p);
					}
					else
					{
						cout << "INVALID OR UNKNOWN LINE IGNORED AT " << Count << endl;
					}
				}
			} // end of while loop
		}

		model.close();
	}
};

#endif // _OBJLOAD_H
