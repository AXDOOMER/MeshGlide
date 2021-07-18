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
// graph.cpp
// Graph of nodes with their connected neighbors

#include "graph.h"
#include "vecmath.h"

#include <algorithm>
#include <vector>
#include <stdexcept>
using namespace std;

Graph::Graph()
{
	// Empty
}

bool Graph::Add(Float3 node, vector<Float3> neighbors)
{
	vector<Float3>::iterator it = find(nodes_.begin(), nodes_.end(), node);

	if (it != nodes_.end())
	{
		// Element found
		unsigned int i = distance(nodes_.begin(), it);
		vector<Float3>* n = &neighbors_[i];
		n->insert(n->end(), neighbors.begin(), neighbors.end());

		return true;
	}

	// Element not found
	nodes_.push_back(node);
	neighbors_.push_back(neighbors);

	return false;
}

bool Graph::Add(Float3 node)
{
	vector<Float3>::iterator it = find(nodes_.begin(), nodes_.end(), node);

	if (it != nodes_.end())
	{
		// Element found
		return true;
	}

	vector<Float3> empty;

	// Element not found
	nodes_.push_back(node);
	neighbors_.push_back(empty);

	return false;
}

vector<Float3> Graph::GetNeighbors(const Float3 node)
{
	vector<Float3>::iterator it = find(nodes_.begin(), nodes_.end(), node);

	if (it != nodes_.end())
	{
		// Element found
		unsigned int i = distance(nodes_.begin(), it);
		return neighbors_[i];
	}

	// Element not found
	throw out_of_range("Node supplied to Graph::GetNeighbors not found in graph.");
}

Float3 Graph::Get(const unsigned int index)
{
	if (index < nodes_.size())
	{
		return nodes_.at(index);
	}

	// Range is wrong
	throw out_of_range("Invalid index supplied to Graph::Get");
}

bool Graph::Remove(const Float3 node)
{
	vector<Float3>::iterator it = find(nodes_.begin(), nodes_.end(), node);

	if (it != nodes_.end())
	{
		// Element found
		unsigned int i = distance(nodes_.begin(), it);
		nodes_.erase(nodes_.begin() + i);
		neighbors_.erase(neighbors_.begin() + i);

		return true;
	}

	// Element not found
	return false;
}

unsigned int Graph::Size() const
{
	return nodes_.size();
}

// Destructor
Graph::~Graph()
{
	// Empty
}
