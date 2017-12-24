#!/bin/bash

# Set the working directory to the script's directory
cd $(dirname "$0")
cd Project

# Compile the source files and execute
g++ main.cpp command.cpp textread.cpp viewdraw.cpp things.cpp texture.cpp vecmath.cpp -std=c++11 -lglfw -lGL -lGLU -lglut -lSDL2 -lSDL2_image -o FPS_simple && ./FPS_simple $@
