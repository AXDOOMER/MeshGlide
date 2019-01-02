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
// config.cpp
// Configuration file handling

#include "config.h"
#include "strutils.h"

#include <string>
#include <map>
#include <iostream>
#include <fstream>
using namespace std;

Config* Config::instance_ = nullptr;

Config::Config()
{
	// Empty
}

bool Config::Init(const string& file)
{
	ifstream configFile;
	configFile.open(file);

	if (configFile.is_open())
	{
		string line;
		unsigned int count = 0;

		while (!configFile.eof())
		{
			getline(configFile, line);
			count++;

			vector<string> s = Split(line, '\t');

			settings_.insert(pair<const string&, const string&>(s[0], s[1]));
		}

		configFile.close();
		cout << "Number of config file lines loaded: " << count << endl;
		return true;
	}

	return false;
}

bool Config::Save(const string& file)
{
	ofstream configFile;
	configFile.open(file);

	if (configFile.is_open())
	{
		bool first = true;

		for (auto& e: settings_)
		{
			if (!first)
				configFile << endl;
			else
				first = false;

			// Write setting
			configFile << e.first << '\t' << e.second;
		}

		configFile.close();
		return true;
	}

	return false;
}

bool Config::Add(const string& key, const string& value)
{
	if (settings_.find(key) == settings_.end())
	{
		settings_.insert(pair<const string&, const string&>(key, value));
		return true;
	}

	return false;
}

string Config::Get(const string& key)
{
	// Will throw 'std::out_of_range' if key is not found
	return settings_.at(key);
}

int Config::GetInt(const string& key)
{
	// Will throw 'std::out_of_range' if key is not found
	return stoi(settings_.at(key));
}

bool Config::GetBool(const string& key)
{
	// Will throw 'std::out_of_range' if key is not found
	return (bool)stoi(settings_.at(key));
}

Config* Config::Instance()
{
	if (!instance_)
		instance_ = new Config;
	return instance_;
}

void Config::DestroyInstance()
{
	delete instance_;
	instance_ = nullptr;
}

void Config::Purge()
{
	settings_.clear();
}

// Destructor
Config::~Config()
{
	// Empty
}
