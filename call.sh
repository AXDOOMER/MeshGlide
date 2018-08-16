#!/bin/bash

# How to setup this: https://baptiste-wicht.com/posts/2011/09/profile-c-application-with-callgrind-kcachegrind.html

# Set the working directory to the script's directory
cd $(dirname "$0")

# Valgrind and callgrind
valgrind --tool=callgrind ./Project/MeshGlide -level test.txt -seed 1
