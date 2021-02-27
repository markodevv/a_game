@echo off

set CommonCompilerFlags=-DEBUG -Gm- -GR- -EHa- -Od -Oi -WX -W3 -wd4201 -wd4100 -wd4189 -DGAME_DEBUG=1 -FC -Z7 -MTd
set CommonLinkerFlags= /INCREMENTAL:NO /OPT:REF winmm.lib GLFW.lib glad.lib user32.lib gdi32.lib shell32.lib dsound.lib
set IncludePaths= /I ../third_party/glfw/include/ /I ../third_party/glad/include/


IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

REM 64-bit build
del *.pdb > NUL 2> NUL
cl  %CommonCompilerFlags% /LD ..\src\game.cpp %IncludePaths% /link /INCREMENTAL:NO /OPT:REF /PDB:game_%date:~-4,4%%date:~-10,2%%date:~-7,2%_%time:~0,2%%time:~3,2%%time:~6,2%.pdb 
cl  %CommonCompilerFlags% ..\src\windows_platform.cpp  %IncludePaths% /link %CommonLinkerFlags% /SUBSYSTEM:windows 
popd
