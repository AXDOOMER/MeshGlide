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
// config.h
// Configuration file handling

#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>
using namespace std;

class Config
{
private:
	// <name> <value>
	map<string, string> settings_;

	static Config* instance_;

	Config();
	~Config();	// Prevent unwanted destruction, so private.

public:
	bool Init(const string& file);
	bool Save(const string& file);

	bool Add(const string& key, const string& value);
	void Purge();
	string Get(const string& key);
	int GetInt(const string& key);
	bool GetBool(const string& key);

	static Config* Instance();

	// Resets the "instance_" pointer
	static void DestroyInstance();
};

#endif	// CONFIG_H
