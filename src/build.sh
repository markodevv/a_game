#!/bin/bash

if [ ! -d "../build" ]; then
    mkdir ../build
fi
pushd ../build
include_paths="-I ../third_party/stb/"
lib_dirs="-L../third_party/stb/build"
source_files="../src/linux_platform.cpp";
disabled_warnings="-Wno-write-strings -Wno-unused-function"
libraries="-lm -lpthread -lX11 -lGL -ldl -l:stb_truetype.a -l:stb_image.a"

gcc -Wall $disabled_warnings -std=c++11 -g $include_paths -DGAME_DEBUG=1 $source_files -o game $lib_dirs $libraries
popd

