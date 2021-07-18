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
// bfs.h
// Breadth-first search algorithm

#ifndef BFS_H
#define BFS_H

#include "vecmath.h"
#include "graph.h"

#include <utility>
#include <algorithm>
#include <vector>
#include <cassert>
#include <iostream>

using namespace std;

vector<Float3> BFS(Graph graph, Float3 node, Float3 dest)
{
	vector<Float3> visited;
	vector<Float3> visitedFrom;
	vector<Float3> queue;

	visited.push_back(node);
	visitedFrom.push_back(node);  // For the first element, "visited" == "visited form"
	queue.push_back(node);        // Enqueue source node

	while (!queue.empty())
	{
		// Return then pop first element
		Float3 s = queue.front();
		queue.erase(queue.begin());
		cout << "(" << s.x << "," << s.y << ") " << endl;

		// We'll enqueue neighbors nodes
		for (auto& neighbor: graph.GetNeighbors(s))
		{
			vector<Float3>::iterator it = find(visited.begin(), visited.end(), neighbor);

			if (it == visited.end()) {
				// Not found, so not already visited

				visited.push_back(neighbor);    // Mark as visited
				queue.push_back(neighbor);      // Enqueue

				// Save visited from to traceback the path later.
				// For i, an index in the visited list, you'll get the 
				// neighbors at the same i in the visitedFrom list.
				visitedFrom.push_back(s);

				// Destionation found, a path is possible. This path will be returned.
				if (queue.back() == dest)
				{
					cout << "Found destination" << endl;

					// Create path
					vector<Float3> path;
					Float3 current = queue.back();
					path.push_back(current);
					while (!(current == node))
					{
						it = find(visited.begin(), visited.end(), current);

						// This assertion shall never fail
						// The path will always be in made of visited nodes
						assert(it != visited.end());

						unsigned int i = distance(visited.begin(), it);
						current = visitedFrom[i];
						path.push_back(current);
					}

					reverse(path.begin(), path.end());
					return path;
				}
			}
		}
	}

	vector<Float3> empty;
	cout << "Destination not found" << endl;
	return empty;
}

#endif	// BFS_H
