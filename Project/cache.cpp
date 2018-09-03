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
// cache.cpp
// Cache that makes use of a map and manages the textures.
// It uses the singleton pattern.

#include "cache.h"
#include "texture.h"

#include <map>
#include <string>
using namespace std;

Cache* Cache::instance_ = nullptr;

Cache::Cache()
{
	// Empty
}

bool Cache::Add(const string& name, bool enableFiltering)
{
	if (store_.find(name) == store_.end())
	{
		store_.insert(pair<const string&, Texture*>(name, new Texture(name, enableFiltering)));
		return true;
	}

	return false;
}

Texture* Cache::Get(const string& key)
{
	// Will throw 'std::out_of_range' if key is not found
	return store_.at(key);
}

unsigned int Cache::Size() const
{
	return store_.size();
}

Cache* Cache::Instance()
{
	if (!instance_)
		instance_ = new Cache;
	return instance_;
}

void Cache::DestroyInstance()
{
	delete instance_;	// The cache will be emptied
	instance_ = nullptr;
}

// Destructor
Cache::~Cache()
{
	// Iterate and delete elements of the map
	for (auto& e: store_) {
		delete e.second;
	}
}
