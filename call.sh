#!/bin/bash

# Set the working directory to the script's directory
cd $(dirname "$0")

# Valgrind and callgrind
valgrind --tool=callgrind ./Project/MeshGlide -level test.txt -seed 1
