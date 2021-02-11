#!/bin/bash

mkdir bin
pushd bin/
include_paths="-I ../third_party/glfw/include/ -I ../third_party/glad/include/"
clang++  -std=c++11 $include_paths -g ../src/main.cpp -o game -lpthread -lGL -ldl -L. -lGLFW -lglad
popd

