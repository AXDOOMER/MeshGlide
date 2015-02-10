// textread.cpp
//Copyright (C) 2014 Alexandre-Xavier Labonté-Lamoureux
//
//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.

// Handles text data reading from file & writing
// Used for config files, levels and demo files.

// basic file operations
#include <iostream>
#include <string>
#include <fstream>

#include "things.h"

using namespace std;

//Load le niveau et retourne son pointeur
Level* F_LoadLevel(string LevelName)
{
	Level* Current = new Level;

	string FileName = LevelName + ".txt";       //Added to compile in Code::Blocks,
	ifstream LevelFile(FileName.c_str());   //can't do it all on this line.
	if (LevelFile.is_open())
	{
		int Count = 0;
		string Line;
		while (!LevelFile.eof())     //Read the entire file until the end
		{
			getline(LevelFile, Line);
			cout << Line << '\n';

			//Serious business
			int Position = Line.find_first_of(":;0123456789");
			string Content = Line.substr(0, Position);
			string id;
			int Number;

			if (Line.find("level:") != string::npos)
			{
				Count++;

				while (Line.find("}") == string::npos)
				{
					getline(LevelFile, Line);
					cout << Line << '\n';

					if (Line.find("{") != string::npos)
					{
						// Go to the next line right now
						//getline(LevelFile, Line);
						//Count++;
					}
					else if (Line.find("name: ") != string::npos)
					{
						Current->Name = (Line.substr(Line.find("name: "), Line.find(';'))).c_str();
					}
					else if (Line.find("fog: ") != string::npos)
					{
						Current->Fog = atoi((Line.substr(Line.find("fog: "), Line.find(';'))).c_str());
					}
					else if (Line.find("}") != string::npos)
					{
						// DEFINITION EXITED
						// We wille exit next time
					}
					Count++;
				}
			}
			else if (Line.find("wall") != string::npos && Line.find(":") != string::npos)
			{
				Count++;

				while (Line.find("}") == string::npos)
				{
					getline(LevelFile, Line);
					cout << Line << '\n';

					if (Line.find("{") != string::npos)
					{
						// Go to the next line right now
						//getline(LevelFile, Line);
						//Count++;
					}
					else if (Line.find("x: ") != string::npos)
					{
						Current->ptrWalls->resize(Current->ptrWalls->size() + 1);
						Current->ptrWalls->at(Current->ptrWalls->size() - 1).ptrVertices->push_back(atoi((Line.substr(Line.find("x: "), Line.find(';'))).c_str()));
					}
					else if (Line.find("y: ") != string::npos)
					{
						Current->ptrWalls->resize(Current->ptrWalls->size() + 1);
						Current->ptrWalls->at(Current->ptrWalls->size() - 1).ptrVertices->push_back(atoi((Line.substr(Line.find("y: "), Line.find(';'))).c_str()));
					}
					else if (Line.find("z: ") != string::npos)
					{
						Current->ptrWalls->resize(Current->ptrWalls->size() + 1);
						Current->ptrWalls->at(Current->ptrWalls->size() - 1).ptrVertices->push_back(atoi((Line.substr(Line.find("z: "), Line.find(';'))).c_str()));
					}
					else if (Line.find("}") != string::npos)
					{
						// We wille exit next time
					}
					Count++;
				}
			}
			//else if (Content == "wall")
			//{
			//	id = Line.substr(Position, Line.find_first_of(":;"));
			//	id = id.substr(0, id.size() - 1);
			//	cout << "--------------" << id << endl;

			//	Count++;
			//}
			//else if (Content == "floor")
			//{
			//	id = Line.substr(Position, Line.find_first_of(":;"));
			//	id = id.substr(0, id.size() - 1);
			//	cout << "--------------" << id << endl;

			//	Count++;
			//}
			//else if (Content == "slope")
			//{
			//	id = Line.substr(Position, Line.find_first_of(":;"));
			//	id = id.substr(0, id.size() - 1);
			//	cout << "--------------" << id << endl;

			//	Count++;
			//}
			//else if (Content == "plane")
			//{
			//	id = Line.substr(Position, Line.find_first_of(":;"));
			//	id = id.substr(0, id.size() - 1);
			//	cout << "--------------" << id << endl;

			//	Count++;
			//}
			//else if (Line.find("thing"))
			//{
			//	Line.find_first_of("_");
			//	id = Line.substr(Line.find("_"), Line.find(":"));
			//	cout << "--------------" << id << endl;

			//	Count++;
			//}
			else
			{
				Count++;
			}

		}
		cout << "Read " << Count << " lines from file. " << endl;
		LevelFile.close();
	}
	else
	{
		delete Current;		// Erase because it's no longer useful
		cout << "Unable to open level info!" << endl;
	}
	return Current;	//Doit retourner un pointeur sur le niveau
}

