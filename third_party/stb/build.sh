#!/bin/bash

if [ ! -d "build" ]; then
    mkdir build
fi
pushd build

gcc -Wall -std=c++11 -c ../stb_image.cpp
gcc -Wall -std=c++11 -c ../stb_image_write.cpp
gcc -Wall -std=c++11 -c ../stb_truetype.cpp

popd
