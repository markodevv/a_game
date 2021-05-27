#!/bin/bash

if [ ! -d "../build" ]; then
    mkdir ../build
fi
pushd ../build
include_paths="-I ../third_party/stb/ -I../third_party/obj_parser/"
lib_dirs="-L../third_party/stb/build"
source_files="../src/linux_platform.cpp";
disabled_warnings="-Wno-unused-variable -Wno-write-strings -Wno-unused-function -Wno-switch -Wno-unused-but-set-variable"
linux_libraries="-lm -lpthread -lX11 -lGL -ldl -l:stb_image.a -l:stb_truetype.a"
game_libraries="-lm"

gcc -Wall $disabled_warnings -std=c++11 -g $include_paths -DPLATFORM_LINUX=1 -DGAME_DEBUG=1 $source_files -o linux_platform $lib_dirs $linux_libraries
gcc -Wall $disabled_warnings -std=c++11 -fpic -g $include_paths -DPLATFORM_LINUX=1 -DGAME_DEBUG=1 -c ../src/game.cpp  $lib_dirs $game_libraries
gcc -shared -o libgame.so game.o
popd

