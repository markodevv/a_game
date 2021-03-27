@echo off

set win32_compiler_flags=-DEBUG -Gm- -GR- -EHa- -Od -Oi -WX -W3 -wd4201 -wd4100 -wd4189 -DGAME_DEBUG=1 -FC -Z7 -MTd -nologo
set win32_linker_flags= /INCREMENTAL:NO /OPT:REF opengl32.lib winmm.lib user32.lib gdi32.lib shell32.lib dsound.lib
set game_linker_flags=/INCREMENTAL:NO /OPT:REF /PDB:game_%date:~-4,4%%date:~-10,2%%date:~-7,2%_%time:~0,2%%time:~3,2%%time:~6,2%.pdb


IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

REM 64-bit build
del *.pdb > NUL 2> NUL
cl  %win32_compiler_flags% /LD ..\src\game.cpp /link %game_linker_flags%
cl  %win32_compiler_flags% ..\src\windows_platform.cpp /link %win32_linker_flags%  
popd
