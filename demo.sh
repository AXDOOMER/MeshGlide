#!/bin/bash

# Set the working directory to the script's directory
cd $(dirname "$0")
cd KillBox

# Compile the source files and execute
g++ main.cpp command.cpp textread.cpp viewdraw.cpp things.cpp texture.cpp -std=c++11 -lglfw -lGL -lGLU -lglut -lSDL -o FPS_test && ./FPS_test -playdemo rec.txt $@
