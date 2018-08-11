// Copyright (C) 2018 Alexandre-Xavier Labonté-Lamoureux
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
// netowrk.h
// Networking component

#include <zmq.hpp>
#include <vector>
#include <string>

using namespace std;
using namespace zmq;

class Network
{
private:
	socket_t* sock_;
	context_t* context_;
	unsigned int id_;		// For 4 players, 0-3

public:
	Network();
	~Network();

	bool enabled();
	unsigned int player();

	vector<unsigned char> receive();
	void send(vector<unsigned char> message);

	void startServer(string port);
	void connectClient(string location);
};

