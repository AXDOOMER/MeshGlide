// Copyright (C) 2021 Alexandre-Xavier Labonté-Lamoureux
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
// graph.h
// Graph of nodes with their connected neighbors

#ifndef GRAPH_H
#define GRAPH_H

#include "vecmath.h"

#include <algorithm>
#include <vector>
using namespace std;

class Graph
{
private:
	// Nodes and their neighbors
	vector<Float3> nodes_;
	vector<vector<Float3>> neighbors_;

public:
	Graph();
	~Graph();

	bool Add(Float3 node, vector<Float3> neighbors);
	bool Add(Float3 node);
	bool Remove(const Float3 node);

	// Returns neighbors
	vector<Float3> GetNeighbors(const Float3 node);
	// Returns the node at this index. Useful to cycle through nodes.
	Float3 Get(const unsigned int index);

	unsigned int Size() const;
};

#endif	// GRAPH_H
