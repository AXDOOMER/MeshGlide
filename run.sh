#!/bin/bash

# Set the working directory to the script's directory
cd $(dirname "$0")
cd Project

# Compile the source code files and execute the program
if [ "$#" -ge 1 ] && [ "$1" == "release" ]; then
	echo "Building release"
	shift
	echo "MeshGlide args: $@"
	g++ *.cpp -std=c++11 -O2 -s -Wall -Wextra -lglfw -lGL -lGLU -lSDL2 -lSDL2_image -o MeshGlide && ./MeshGlide $@
else
	echo "Building debug"
	echo "MeshGlide args: $@"
	g++ *.cpp -std=c++11 -O0 -g -Wall -Wextra -lglfw -lGL -lGLU -lSDL2 -lSDL2_image -o MeshGlide && ./MeshGlide $@
fi
