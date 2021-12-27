@echo off
cd /d %~dp0

set PROJECT="Turbulence"
set OUTPUT="./build/%PROJECT%_win.exe"

set COMPILER_FLAGS= ^
/WX /W4 /WL /wd4189 /wd4201 /wd4312 /wd4456 /wd4127 /wd4100 /wd4505 /wd4702 /wd4701 /we4457 /we4456 ^
/Zi /Oi  /nologo /MT /EHsc-

set LINKER_FLAGS=/OUT%OUTPUT%

set SRC=^
src/Main.cpp ^
dependencies/Physics2D/src/Collider.cpp ^
dependencies/Physics2D/src/PhysicsWorld.cpp ^
dependencies/Physics2D/src/RigidBody.cpp ^
dependencies/Physics2D/src/Geometry.cpp  ^
dependencies/Physics2D/src/SAT.cpp ^
dependencies/Physics2D/src/Ray.cpp ^
dependencies/Math/src/Math_Vector.cpp ^
dependencies/Math/src/Math_Random.cpp ^
dependencies/Noise/src/Noise.cpp

set INCLUDE_FOLDERS= ^
/I ./dependencies/PlatformLayer/include/ ^
/I ./dependencies/OpenGL_Renderer/include/ ^
/I ./dependencies/LinkedList/src/ ^
/I ./dependencies/Array/src/ ^
/I ./dependencies/HashTable/src/ ^
/I ./dependencies/Math/src/ ^
/I ./dependencies/Physics2D/src/ ^
/I ./dependencies/stb/ ^
/I ./dependencies/Noise/src/

set LIBS= ^
./dependencies/PlatformLayer/lib/PlatformLayer_win32.lib ^
./dependencies/OpenGL_Renderer/lib/Renderer.lib ^
./dependencies/OpenGL_Renderer/dependencies/GLEW/lib/glew32s.lib ^
Opengl32.lib ^
User32.lib ^
Gdi32.lib ^
Ole32.lib ^
Shlwapi.lib  


set PREPROCESSOR= ^
/D GLEW_STATIC=1 ^
/D GLEW_NO_GLU=1 ^
/D _CRT_SECURE_NO_WARNINGS=1

set LINKER_FLAGS= ^
/link /OUT:%OUTPUT% /Incremental

IF NOT EXIST .\build mkdir build

set START=%time%
cl /Fo".//build//" %PREPROCESSOR% %COMPILER_FLAGS% %SRC% %LIBS% %INCLUDE_FOLDERS%  %LINKER_FLAGS% 
