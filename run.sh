#!/bin/bash

# Name of the output file
EXENAME=MeshGlide

# Set the working directory to the script's directory
cd $(dirname "$0")
cd Project

# Compile the source code files and execute the program by default
if [ "$#" -ge 1 ] && [ "$1" == "cppcheck" ]; then
	cd ..
	cppcheck --error-exitcode=1 -j2 -UTESTING -Iopl -Isrc -Isrc/Project Project/
elif [ "$#" -ge 1 ] && [ "$1" == "info" ]; then
	if [ -f $EXENAME ]; then
		echo -n "File size in bytes is: "
		du -b $EXENAME | cut -f 1
		md5sum $EXENAME
	else
		echo "File not found!"
	fi
elif [ "$#" -ge 1 ] && [ "$1" == "release" ]; then
	echo "Building release"
	shift
	echo "$EXENAME args: $@"
	g++ *.cpp -std=c++11 -O2 -s -Wall -Wextra -lglfw -lGL -lGLU -lSDL2 -lSDL2_image -lzmq -o $EXENAME && ./$EXENAME $@
elif [ "$#" -ge 1 ] && [ "$1" == "gdb" ]; then
	echo "Building and running on GDB"
	shift
	echo "$EXENAME args: $@"
	g++ *.cpp -std=c++11 -O0 -Wall -Wextra -lglfw -lGL -lGLU -lSDL2 -lSDL2_image -lzmq -o $EXENAME && gdb --args ./$EXENAME $@
else
	echo "Building default"
	echo "$EXENAME args: $@"
	g++ *.cpp -std=c++11 -O0 -Wall -Wextra -lglfw -lGL -lGLU -lSDL2 -lSDL2_image -lzmq -o $EXENAME && ./$EXENAME $@
fi
