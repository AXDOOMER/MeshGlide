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
// cache.h
// Generic cache that's a facade for a map and takes pointers to a value.
// It handles the deletition when the cache object is destroyed.
// It uses the singleton pattern.
// The method that returns the previous element is not thread safe.

#ifndef CACHE_H
#define CACHE_H

#include "texture.h"

#include <map>
#include <string>
using namespace std;

class Cache
{
private:
	map<string, Texture*> store_;
	string prev_;	// Previous used key when doing a Get()

	static Cache* instance_;

	Cache();
	~Cache();	// Prevent unwanted destruction

public:
	void Add(const string& key, Texture* data);

	bool Has(const string& key) const;

	// TODO: Remove 'Add' and 'Has'. This could be all done in 'Get'.

	Texture* Get(const string& key);

	unsigned int Size() const;

	string Previous() const;

	static Cache* Instance();

	// Resets the "instance_" pointer
	static void DestroyInstance();
};

#endif	// CACHE_H
