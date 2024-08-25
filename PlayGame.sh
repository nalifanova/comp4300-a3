#!/bin/bash

export PATH=$PATH:`dirname $(which cmake)`
# create directories and move it there
mkdir -p GAME11/BUILD && cd GAME11/BUILD

# Run cmake and wait for the result
if cmake ../..; then
    if make; then # Happy case with build
        ./bin/Game11
    else
        echo "Something went wrong with the build process."
    fi
else
    echo "Error: CMake failed."
fi
