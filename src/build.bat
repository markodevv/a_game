@echo off

set include_paths=  -I ..\third_party\stb\ -I ..\third_party\glad\include\

set complie_flags= %include_paths% -DEBUG -Gm- -GR- -EHa- -Od -Oi -WX -W3 -wd4996 -wd4311 -wd4302 -wd4201 -wd4100 -wd4189 -wd4129 -wd4244 -DPLATFORM_WIN32=1 -DGAME_DEBUG=1 -DGAME_PROFILE=0 -FC -Z7 -MTd -nologo

set lib_paths= /LIBPATH:..\third_party\stb\build /LIBPATH:..\third_party\glad\build

set win32_linker_flags=/OPT:REF %lib_paths% opengl32.lib winmm.lib user32.lib gdi32.lib shell32.lib dsound.lib

set game_linker_flags=/OPT:REF /PDB:game_%random%.pdb %lib_paths% 

set opengl_linker_flags=/OPT:REF  %lib_paths% opengl32.lib glad.lib  gdi32.lib user32.lib

@RD /S /Q ".\generated"
mkdir .\generated

data_desk --custom ../meta/datadesk_layer.dll memory.mdesk math.mdesk input.mdesk renderer.mdesk debug.mdesk dev_ui.mdesk game.mdesk

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

REM 64-bit build
del *.pdb > NUL 2> NUL

REM windows OS OpenGL build
REM cl  %complie_flags%  /LD ..\src\windows_opengl.cpp /link %opengl_linker_flags%

REM game dll build
cl  %complie_flags% /LD ..\src\game.cpp /link %game_linker_flags%

REM windows OS build
cl  %complie_flags%  /Fecool_game.exe ..\src\windows_platform.cpp  /link %win32_linker_flags%

popd

