@echo off

set CommonCompilerFlags=-DEBUG -Gm- -GR- -EHa- -Od -Oi -WX -W3 -wd4201 -wd4100 -wd4189 -DGAME_DEBUG=1 -FC -Z7 -MTd
set CommonLinkerFlags= -opt:ref GLFW.lib glad.lib opengl32.lib user32.lib gdi32.lib shell32.lib dsound.lib
set IncludePaths= /I ../third_party/glfw/include/ /I ../third_party/glad/include/

REM TODO - can we just build both with one exe?

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

REM 64-bit build
cl  %CommonCompilerFlags% ..\src\windows_platform.cpp %IncludePaths% /link %CommonLinkerFlags% 
popd
