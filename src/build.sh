#!/bin/bash

if [ ! -d "../build" ]; then
    mkdir ../build
fi
pushd ../build
include_paths="-I ../third_party/glfw/include/ -I ../third_party/glad/include/"
source_files="../src/linux_platform.cpp";
clang++ -Wall -std=c++11 -g $include_paths -DGAME_DEBUG=1 $source_files -o game -lpthread -lGL -ldl -L. -lGLFW -lglad
popd

