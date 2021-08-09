@echo off
if not exist build mkdir build
pushd build
cl /nologo /Zi /I..\source\ ..\docs_gen\docs_gen.c
docs_gen.exe ../source/data_desk.h
popd