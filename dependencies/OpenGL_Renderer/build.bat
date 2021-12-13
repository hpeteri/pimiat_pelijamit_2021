@echo off

call ".\shell.bat"
  
setlocal
cd /d %~dp0

SET PROJECT=RENDERER
SET OUTPUT=".//build//Renderer.lib"
  
SET COMPILER_FLAGS= ^
/WX /W4 /WL /wd4189 /wd4201 /wd4312 /wd4456 /wd4127 /wd4100 /wd4505 /wd4702 /wd4701 /we4457 /we4456 ^
/Zi /Gm /Oi  /nologo /MT /EHsc- 

SET PREPROCESSOR= ^
/D GLEW_STATIC=1 /D _CRT_SECURE_NO_WARNINGS=1

SET SRC= ^
.\src\RENDERER_RendererMain.cpp

SET INCLUDE_FOLDERS= ^
/I ./dependencies/GLEW/include ^
/I ./dependencies/Math/src/ ^
/I ./dependencies/LinkedList/src/ ^
/I ./dependencies/Array/src/ ^
/I ./dependencies/HashTable/src/ ^
/I ./dependencies/stb/


IF NOT EXIST .\lib mkdir lib
IF NOT EXIST .\include mkdir include
IF NOT EXIST .\build mkdir build

    
echo !!! Building '%PROJECT%' !!!
echo %SRC%
cl /Fo".//build//" /c %SRC% %INCLUDE_FOLDERS% %PREPROCESSOR% %COMPILER_FLAGS%
lib .//build//RENDERER_RendererMain.obj /OUT:%OUTPUT%

copy .\src\*.h .\include\
copy .\build\Renderer.lib .\lib\Renderer.lib
