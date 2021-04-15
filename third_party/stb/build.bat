@echo off

set compiler_flags= -Gm- -GR- -EHa- -O2 -Oi -WX -W3 -wd4996 -wd4311 -wd4302 -wd4201 -wd4100 -wd4189 -wd4129 -FC -Z7 -MTd -nologo


IF NOT EXIST build mkdir build
pushd build

REM 64-bit build
cl  %compiler_flags% /LD ..\stb_image.cpp
cl  %compiler_flags% /LD ..\stb_image_write.cpp
cl  %compiler_flags% /LD ..\stb_truetype.cpp

popd
