@echo off

set win32_compiler_flags=-I ..\third_party\assimp\include -DEBUG -Gm- -GR- -EHa- -Od -Oi -WX -W3 -wd4996 -wd4311 -wd4302 -wd4201 -wd4100 -wd4189 -wd4129 -DGAME_DEBUG=1 -FC -Z7 -MTd -nologo
set win32_linker_flags=/INCREMENTAL:NO /OPT:REF  opengl32.lib winmm.lib user32.lib gdi32.lib shell32.lib dsound.lib assimp-vc142-mt.lib
set game_linker_flags=/INCREMENTAL:NO /OPT:REF /PDB:game_%random%.pdb 


IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

REM 64-bit build
del *.pdb > NUL 2> NUL
cl  %win32_compiler_flags% /LD ..\src\game.cpp /link %game_linker_flags%
cl  %win32_compiler_flags% ..\src\windows_platform.cpp /link %win32_linker_flags%  
popd
