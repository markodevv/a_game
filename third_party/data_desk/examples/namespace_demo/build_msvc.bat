@echo off
start /b /wait "" "cl.exe" /Zi -nologo -I ..\..\source\ namespace_demo.c /LD /link /out:namespace_demo.dll
..\..\build\data_desk.exe --custom namespace_demo.dll data_v1.ds data_v2.ds
start /b /wait "" "cl.exe" /Zi -nologo -I ..\..\source\ conversion.c /link /out:conversion.exe
conversion.exe
