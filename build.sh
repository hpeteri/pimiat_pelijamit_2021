#!/bin/bash

./dependencies/PlatformLayer/build.sh
./dependencies/OpenGL_Renderer/build.sh


PROJECT="Pimiät_Pelijamit_2021_Henrik_Peteri"

OUTPUT="./build/"$PROJECT".exe"

WARNINGS="-Werror"
EXECUTABLE_NAME="./main.exe"
SRC="src/Main.cpp
dependencies/Physics2D/src/PhysicsWorld.cpp
dependencies/Physics2D/src/RigidBody.cpp 
dependencies/Physics2D/src/Geometry.cpp  
dependencies/Physics2D/src/Collider.cpp
dependencies/Physics2D/src/SAT.cpp
dependencies/Physics2D/src/Ray.cpp
dependencies/Math/src/Math_Vector.cpp
dependencies/Math/src/Math_Random.cpp
dependencies/Noise/src/Noise.cpp
"
INCLUDE_FOLDERS="-I ./dependencies/PlatformLayer/include/ 
                 -I ./dependencies/OpenGL_Renderer/include/     
                 -I ./dependencies/LinkedList/src/
                 -I ./dependencies/Array/src/
                 -I ./dependencies/HashTable/src/
                 -I ./dependencies/Math/src/
                 -I ./dependencies/Physics2D/src/
                 -I ./dependencies/stb/
                 -I ./dependencies/Noise/src/
"

LIBS="-lX11 -pthread -lGL
      ./dependencies/PlatformLayer/lib/PlatformLayer_linux.a
      ./dependencies/OpenGL_Renderer/lib/Renderer.a
"

PREPROCESSOR="-D GLEW_STATIC=1 -D GLEW_NO_GLU=1"

pushd "`dirname ${BASH_SOURCE[0]}`"

# Create a build folder where our executable will be placed
if [ ! -d "build" ]
then
    mkdir build
fi


g++ $PREPROCESSOR $COMPILER_FLAGS $SRC $LIBS $INCLUDE_FOLDERS $LIBS -o $OUTPUT 


popd
exit 0
