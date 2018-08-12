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
// network.cpp
// Networking component

#include <zmq.hpp>
#include <string>	// to_string
#include <iostream>	// cout
#include <vector>
#include <cstring>	// memcpy

#include "network.h"

using namespace std;
using namespace zmq;

Network::Network()
{
	id_ = 0;
	sock_ = nullptr;
	context_ = nullptr;
}

Network::~Network()
{
	if (sock_)
	{
		delete sock_;
		delete context_;
	}
}

bool Network::enabled()
{
	return sock_ != nullptr;
}

unsigned int Network::myPlayer()
{
	return id_;
}

void Network::send(const vector<unsigned char>& message)
{
	message_t request(message.size());
	memcpy(request.data(), &message[0], message.size());

	sock_->send(request);
}

vector<unsigned char> Network::receive()
{
	message_t message;
	sock_->recv(&message);

	vector<unsigned char> v(message.size());
	memcpy(v.data(), message.data(), message.size());

	return v;
}

void Network::sendString(const string& s)
{
	message_t message(s.size());
	memcpy(message.data(), s.data(), s.size());

	sock_->send(message);
}

string Network::receiveString()
{
	message_t reply;
	sock_->recv(&reply);

	return string(static_cast<char*>(reply.data()), reply.size());
}

void Network::startServer(const string& port, const string& info)
{
	context_ = new context_t(1);
	sock_ = new socket_t(*context_, ZMQ_REP);

	cout << "Starting local server on port '" << port << "'" << endl;
	sock_->bind("tcp://*:" + port);

	// Init game
	receiveString();
	sendString(info);

	id_ = 0;
}

string Network::connectClient(const string& location)
{
	context_ = new context_t(12);
	sock_ = new socket_t(*context_, ZMQ_REQ);

	cout << "Connecting to server at '" << location << "'" << endl;
	sock_->connect("tcp://" + location);

	// Init game
	sendString("Hello, World!");
	string settings = receiveString();

	id_ = 1;

	return settings;
}
