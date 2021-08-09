@echo off
start /b /wait "" "cl.exe" /Zi -nologo -I ..\..\source\ print_code_generation.c /LD /link /out:print_code_generation.dll
..\..\build\data_desk.exe --custom print_code_generation.dll data.ds