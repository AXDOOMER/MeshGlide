// Copyright (C) 2017 Alexandre-Xavier Labonté-Lamoureux
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

#ifndef CACHE_H
#define CACHE_H

#include <map>
using namespace std;

template <class K, class T>
class Cache
{
private:
	map<K, T*> store_;
	K prev_;	// Previous used key when doing a Get()
public:
	Cache()
	{
		prev_ = "";
	}

	void Add(const K& key, T* data)
	{
		store_.insert(pair<const K&, T*>(key, data));
	}

	bool Has(const K& key) const
	{
		return store_.find(key) != store_.end();
	}

	T* Get(const K& key)
	{
		prev_ = key;
		// Will throw 'std::out_of_range' if key is not found
		return store_.at(key);
	}

	unsigned int Size() const
	{
		return store_.count();
	}

	K Previous() const
	{
		return prev_;
	}

	// Destructor
	~Cache()
	{
		// Iterate and delete elements of the map
		for (auto& e: store_) {
			delete e.second;
		}
	}
};

#endif	// CACHE_H
