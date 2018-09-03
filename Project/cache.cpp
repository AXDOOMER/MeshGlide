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
// Generic cache that's a facade for a map and takes pointers to a value.
// It handles the deletition when the cache object is destroyed.
// It uses the singleton pattern.
// The method that returns the previous element is not thread safe.

#include "cache.h"
#include "texture.h"

#include <map>
#include <string>
using namespace std;

Cache* Cache::instance_ = nullptr;

Cache::Cache()
{
	prev_ = "";
}

void Cache::Add(const string& key, Texture* data)
{
	store_.insert(pair<const string&, Texture*>(key, data));
}

bool Cache::Has(const string& key) const
{
	return store_.find(key) != store_.end();
}

Texture* Cache::Get(const string& key)
{
	prev_ = key;
	// Will throw 'std::out_of_range' if key is not found
	return store_.at(key);
}

unsigned int Cache::Size() const
{
	return store_.size();
}

string Cache::Previous() const
{
	return prev_;
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
