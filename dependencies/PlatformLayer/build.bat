@echo off

call "%APPDATA%\shell.bat" 

cd /d %~dp0

SET PROJECT=PlatformLayer
SET OUTPUT=".//Build//PlatformLayer_win32.lib"
SET PREPROCESSOR= /D _CRT_SECURE_NO_WARNINGS=1 
SET COMPILERFLAGS= ^
/WX /W4 /WL /wd4189 /wd4201 /wd4312 /wd4456 /wd4127 /wd4100 /wd4505 /wd4702 /wd4701 /we4457 /we4456 ^
/Zi /Gm /Oi  /nologo /MT /EHsc- 
SET SRC=.\src\Main.cpp
SET INCLUDE_FOLDERS=/I ./dependencies/GLEW/include/GL/
SET LIBS=""


IF NOT EXIST .\lib mkdir lib
IF NOT EXIST .\include mkdir include
IF NOT EXIST .\build mkdir build

echo !!! Building '%PROJECT%' !!!

cl /Fo".//Build//" -c %PREPROCESSOR% %COMPILER_FLAGS% %SRC% %INCLUDE_FOLDERS%
lib  .\build\main.obj /OUT:%OUTPUT%

echo.
echo Copying '%PROJECT%' files to 'lib' and 'include'
copy .\src\*.h .\include\
copy .\build\PlatformLayer_win32.lib .\lib\

