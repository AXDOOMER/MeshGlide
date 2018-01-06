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
// Generic cache

#ifndef _CACHE_H
#define _CACHE_H

#include <map>
using namespace std;

template <class K, class T>
class Cache
{
private:
	map<K, T*> _store;
	K _prev;	// Previous used key when doing a Get()
public:
	Cache()
	{
		_prev = "";
	}

	void Add(const K& key, T* data)
	{
		_store.insert(pair<const K&, T*>(key, data));
	}

	bool Has(const K& key) const
	{
		return _store.find(key) != _store.end();
	}

	T* Get(const K& key)
	{
		_prev = key;
		// Will throw 'std::out_of_range' if key is not found
		return _store.at(key);
	}

	unsigned int Size() const
	{
		return _store.count();
	}

	K Previous() const
	{
		return _prev;
	}

	// Destructor
	~Cache()
	{
		for (auto& e: _store) {
			delete e.second;
		}
	}
};

#endif	// _CACHE_H
