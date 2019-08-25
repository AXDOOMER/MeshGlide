# Copyright (C) 2019 Alexandre-Xavier Labonté-Lamoureux
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Server for MeshGlide multiplayer
# Acts as a relay between clients which will play together in the same game.

import zmq
import time
from datetime import datetime

#context = zmq.Context.instance()
context = zmq.Context()
client = context.socket(zmq.ROUTER)
client.bind("tcp://*:32456")

count = 0
playerid = 0
maxplayers = 0

orig = None

identities = []

timestr = time.strftime("%Y%m%d-%H%M%S")

print("MeshGlide server v0.1 started")

while True:
	lvlname = ""
	rndseed = 0
	numplyrs = 0

	identity, delimiter, message = client.recv_multipart()

	print("ID: {0}, Delim: {1}, Message: {2}".format(identity, delimiter, message))

	identities.append(identity)

	values = message.decode("ascii")
	values += "\n" + str(playerid)

	splitted = values.split("\n")
	print(splitted)

	if playerid == 0:
		orig = message

		maxplayers = splitted[2]
		print("Max players in game:", maxplayers)

	playerid += 1

	print("teststt**********", maxplayers, playerid)

	if int(maxplayers) == playerid:
		break


while count != int(maxplayers):

	print(datetime.now(), "================")

	message = orig + str.encode("\n" + str(count))

	print("identity: ", identities[count])
	print(message.decode("ascii"))

	client.send_multipart([
		identities[count],
		b"",
		message,
	])

	count += 1

while count > 0:

#	print(".", end = '', flush=True)

	identity, delimiter, message = client.recv_multipart()

	identity2, delimiter2, message2 = client.recv_multipart()

	client.send_multipart([
		identity,
		b"",
		message2,
	])

	client.send_multipart([
		identity2,
		b"",
		message,
	])
