@echo off
if not exist build mkdir build
pushd build
clang-cl -D_CRT_SECURE_NO_DEPRECATE -nologo /Zi -DBUILD_WIN32=1 -DBUILD_LINUX=0 ..\source\data_desk_main.c /link /out:data_desk.exe
popd