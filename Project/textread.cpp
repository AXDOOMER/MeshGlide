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
// textread.cpp
// Handles text data reading from file & writing.
// Used for config files, levels and demo files.

#include "things.h"
#include "vecmath.h" // Float3

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iterator>
using namespace std;

//Load le niveau et retourne son pointeur
Level* F_LoadLevel(const string& LevelName)
{
	Level* Current = new Level;

	string FileName = LevelName;       //Added to compile in Code::Blocks,
	ifstream LevelFile(FileName.c_str());   //can't do it all on this line.
	if (LevelFile.is_open())
	{
		bool blurTextures = false;
		unsigned int Count = 0;
		string Line;
		while (!LevelFile.eof())     //Read the entire file until the end
		{
			getline(LevelFile, Line);

			if (Line.size() > 0)
			{
				Count++;
				//cout << Count << ": " << Line << '\n';

				istringstream buf(Line);
				istream_iterator<string> beg(buf), end;
				vector<string> tokens(beg, end);

				if (tokens.size() > 0 && tokens[0][0] != '#')
				{
					if (tokens[0] == "thing" && tokens.size() == 6)
					{
						if (tokens[1] == "player")
						{
							Current->play = new Player;
							Current->play->pos_.x = atof(tokens[2].c_str());
							Current->play->pos_.y = atof(tokens[3].c_str());
							Current->play->pos_.z = atof(tokens[4].c_str());
							Current->play->Angle = (short)atoi(tokens[5].c_str()) * 91.0222222222f;
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
						for (int i = 9; i < tokens.size(); i += 3)
						{
							Float3 vt;
							vt.x = atof(tokens[i].c_str());
							vt.y = atof(tokens[i+1].c_str());
							vt.z = atof(tokens[i+2].c_str());
							p->Vertices.push_back(vt);
						}

						if (tokens[1] != "NOTEXTURE")
						{
							Current->AddTexture(tokens[1], blurTextures);	// Add texture to cache
							p->Texture = tokens[1];
						}

						p->ComputeWallInfo();			// Comment this line to become a ghost
						Current->planes.push_back(p);
					}
					else if (tokens[0] == "setting" && tokens.size() == 3)
					{
						if (tokens[1] == "blur")
						{
							blurTextures = tokens[2][0] != '0';
						}
						else if (tokens[1] == "skytex")
						{
							Current->AddTexture(tokens[2], blurTextures);
							Current->SkyTexture = tokens[2];
						}
						else if (tokens[1] == "skyele")
						{
							Current->SkyHeigth = atof(tokens[2].c_str());
						}
					}
					else
					{
						cout << "INVALID LINE IGNORED AT: " << Count << endl;
					}
				}
			}
		}

		cout << "Read " << Count << " lines from file. " << endl;
		LevelFile.close();
	}
	else
	{
		delete Current;		// Erase because it's no longer useful
		Current = nullptr;
		cout << "Unable to open level file!" << endl;
	}

	return Current;	//Doit retourner un pointeur sur le niveau
}

