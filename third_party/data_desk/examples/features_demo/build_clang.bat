@echo off
clang-cl -D_CRT_SECURE_NO_DEPRECATE /Zi /nologo -I ../../source/ ./features_demo.c /LD /link /out:features_demo.dll
..\..\build\data_desk.exe --custom features_demo.dll data.ds

