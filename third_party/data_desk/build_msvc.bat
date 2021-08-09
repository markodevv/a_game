@echo off
if not exist build mkdir build
pushd build
cl -nologo /Zi -DBUILD_WIN32=1 -DBUILD_LINUX=0 ..\source\data_desk_main.c /link /out:data_desk.exe
popd