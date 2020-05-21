#!/bin/bash

# Name of the output file
EXENAME=MeshGlide

# Set the working directory to the script's directory
cd $(dirname "$0")
cd Project

# Compile the source code files and execute the program by default
if [ "$#" -ge 1 ] && [ "$1" == "release" ]; then
	echo "Building release"
	shift
	echo "$EXENAME args: $@"
	g++ *.cpp -std=c++14 -O2 -s -Wall -Wextra -lglfw -lGL -lGLU -lSDL2 -lSDL2_image -lzmq -o $EXENAME && ./$EXENAME $@
else
	echo "Building default"
	echo "$EXENAME args: $@"
	g++ *.cpp -std=c++14 -g -Wall -Wextra -lglfw -lGL -lGLU -lSDL2 -lSDL2_image -lzmq -o $EXENAME && ./$EXENAME $@
fi
