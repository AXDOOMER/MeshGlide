#!/usr/bin/python3
# Copyright (C) 2018 Alexandre-Xavier Labonté-Lamoureux
# Converts KillBox maps to MeshGlide maps
# Like MeshGlide and KillBox, this software is distributed under the GPLv3.

import sys

def splitandclean(s):
	s = s.replace(' ', '\t')
	s = s.replace('\n', '')
	s = s.replace(';', '')

	s = s.replace('bmp', 'png')	# Replace because those images where converted

	words = s.split('\t')
	i = 0
	while i < len(words):
		if words[i] == '\t' or words[i] == '':
			del words[i]
		else:
			i = i + 1
	return words

def findarray(arr, s):
	for e in arr:
		if e[0] == s:
			if e[1] == "true":
				return "1"
			elif e[1] == "false":
				return "0"
			return e[1]
	return None

if len(sys.argv) > 1:
	print("#Opening file " + sys.argv[1])
	f = open(sys.argv[1], "r")
	#with open(sys.argv[1], "r") as f:
	emptylines = 0
	while emptylines < 2:
		line = f.readline()
#		print("1- " + line)
		if len(line) > 0:
			emptylines = 0
			words = splitandclean(line)

			if len(words) == 0:
				continue

			if words[0].startswith('#'):
				continue

			if words[0].startswith("floor") or words[0].startswith("ceiling") or words[0].startswith("wall"):
				arr = []
				while True:
					line = f.readline()

#					print("2")
					if len(line) > 0:
#						print(line)
						words = splitandclean(line)
						if len(words) > 0:
							arr.append(words)

					if '}' in line:
						break

#				for e in arr:
#					print(e)

				base = "poly\t" + findarray(arr, "texture:") + '\t' + findarray(arr, "impassable:") + '\t' + findarray(arr, "2sided:")
				base += "\t1\t1\t0\t0\t1"


				for elem in arr:
					if elem[0] == "x:" or elem[0] == "y:" or elem[0] == "z:":
						base += '\t' + str(round(int(elem[1]) / 24, 1))

				print(base)

			elif words[0].startswith("spawn"):
				arr = []
				while True:
					line = f.readline()

#					print("2")
					if len(line) > 0:
#						print(line)
						words = splitandclean(line)
						if len(words) > 0:
							arr.append(words)

					if '}' in line:
						break

				base = "thing\tspawn\t" + str(round(int(findarray(arr, "x:")) / 24,1 )) + '\t' + str(round(int(findarray(arr, "y:")) / 24, 1)) + '\t' + str(round(int(findarray(arr, "z:")) / 24, 1)) + '\t' + findarray(arr, "angle:")
				print(base)

		else:
			emptylines = emptylines + 1
#		if words[0].startswith("thing"):

#		if words[0].startswith("spawn"):

	f.close()
