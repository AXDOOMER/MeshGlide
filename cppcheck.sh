#!/bin/bash

# Set the working directory to the script's directory
cd $(dirname "$0")

# Run Cppcheck
cppcheck --error-exitcode=1 -j2 -UTESTING -Iopl -Isrc -Isrc/Project Project/
