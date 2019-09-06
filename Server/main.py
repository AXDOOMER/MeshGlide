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

port = 32456

context = zmq.Context()
client = context.socket(zmq.ROUTER)
client.bind("tcp://*:" + str(port))

#client.setsockopt(zmq.RCVTIMEO, 5000)
#client.setsockopt(zmq.SNDTIMEO, 5000)

def wait_for_clients(identities):

	game_settings = None
	player_count = 0
	maxplayers = 0

	while True:
		# Wait to receive a message from a client that wants to start a game
		identity, delimiter, message = client.recv_multipart()

		print("ID: {0}, Delim: {1}, Message: {2}".format(identity, delimiter, message))

		identities.append(identity)

		values = message.decode("ascii")

		if player_count == 0:
			newfile.write(str.encode("DUMP FROM MESHGLIDE SERVER 0.1\n" + values + "\n"))

#		values += "\n" + str(player_count)

		splitted = values.split("\n")
		print("splitted: " + str(splitted))

		if player_count == 0:
#			orig = message
			game_settings = message#.decode("ascii")

			maxplayers = splitted[2]
			print("Max players in game:", maxplayers)

			if int(maxplayers) < 2:
				print("Cannot start a game with less than two players.")
				quit()

		player_count += 1

		print("teststt**********", maxplayers, player_count)

		if int(maxplayers) == player_count:
			break

	return game_settings

def reply_to_clients(identities, game_settings):

	count = 0
	maxplayers = game_settings.decode("ascii").split("\n")[2]

	while count != int(maxplayers):

		print(datetime.now(), "================")

		message = game_settings + str.encode("\n" + str(count))

		print("identity: ", identities[count])
		print(message.decode("ascii"))

		client.send_multipart([
			identities[count],
			b"",
			message,
		])

		count += 1

def relay_game_clients(identities):

	count = len(identities)

	while True:

	#	print(".", end = '', flush=True)

		packets = []

		for i in range(count):
			packets.append(client.recv_multipart())

		#identity, delimiter, message = client.recv_multipart()

		#identity2, delimiter2, message2 = client.recv_multipart()

		#TODO the order byte arrays are sorted must match the player ID

		quit = False
		l = [None] * count

		print("count: " + str(count))

		for p in packets:

			message = p[2]

			action_byte = int.from_bytes(message[:1], "little")
			fire = action_byte & 64
			quit1 = action_byte & 128
			plyrid = action_byte & 63

			l[plyrid] = p

			print("test quit bit: ", str(quit), "   and player id is ", str(plyrid), " fire is ", str(fire), "...  byte is ", str(action_byte))

			quit = (quit | quit1) & 128 == 128

#		action_byte2 = int.from_bytes(message2[:1], "little")
#		fire2 = action_byte2 & 64
#		quit2 = action_byte2 & 128
#		plyrid2 = action_byte2 & 63

#		print("test quit bit: ", str(quit2), "   and player id is ", str(plyrid2), " fire is ", str(fire2), "...  byte is ", str(action_byte2))

		

#		if plyrid == 0:
#			newfile.write(message[:7])
#			newfile.write(message2[:7])
#		else:
#			newfile.write(message2[:7])
#			newfile.write(message[:7])

		# Write demo recording
		for p in packets:
			# write the command part of each client's message	
			newfile.write(p[2][:7])
#			p[2][:7]

		newfile.flush()

		for iden in range(len(identities)):

			msg = b""

			for iplyrcmd in range(len(l)):
				if iplyrcmd != iden:
					msg += l[iplyrcmd][2]

			client.send_multipart([
				identities[iden],	# identity
				b"",
				msg,
			])

#		client.send_multipart([
#			identity2,
#			b"",
#			message,
#		])

		if quit:
			print("***** quit message seen *****")
			break




print("MeshGlide server v0.1 started")

while True:

	# init
	identities = []

	print("Waiting for connexions on port " + str(port))

	print("Waiting for a game to start...")

	timestr = time.strftime("%Y%m%d-%H%M%S")
#TODO start recording only after game started (else file remain with 0 bytes)
	newfile = open(timestr + ".rec",'wb')

	game_settings = wait_for_clients(identities)
	reply_to_clients(identities, game_settings)
	relay_game_clients(identities)

print("*****  Done  *****")



