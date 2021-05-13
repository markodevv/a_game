#!/bin/bash

if [ ! -d "../build" ]; then
    mkdir ../build
fi
pushd ../build
include_paths="-I ../third_party/stb/"
lib_dirs="-L../third_party/stb/build"
source_files="../src/linux_platform.cpp";
disabled_warnings="-Wno-write-strings -Wno-unused-function"
libraries="-lm -lpthread -lX11 -lGL -ldl "

gcc -Wall $disabled_warnings -std=c++11 -g $include_paths -DPLATFORM_LINUX=1 -DGAME_DEBUG=1 $source_files -o linux_platform $lib_dirs $libraries
gcc -Wall $disabled_warnings -std=c++11 -fpic -g $include_paths -DPLATFORM_LINUX=1 -DGAME_DEBUG=1 -c ../src/game.cpp  $lib_dirs $libraries
gcc -shared -o libgame.so game.o
popd

