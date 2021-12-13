#!/bin/bash

#Variables
OUTPUT="./build/Renderer.a"
WARNINGS="-Werror"  
COMPILER_FLAGS="-O0"
SRC="src/RENDERER_RendererMain.cpp"

INCLUDE_FOLDERS="-I ./dependencies/GLEW/include/ 
-I ./dependencies/Math/src/
-I ./dependencies/Array/src/
-I ./dependencies/HashTable/src/
-I ./dependencies/LinkedList/src/
-I ./dependencies/stb/"


PREPROCESSOR="-D GLEW_STATIC=1 -D GLEW_NO_GLU=1"

pushd "`dirname ${BASH_SOURCE[0]}`"

if [ ! -d "build" ]
then
    mkdir build
fi

if [ ! -d "include" ]
then
    mkdir include
fi
if [ ! -d "lib" ]
then
    mkdir lib
fi

g++ $PREPROCESSOR $COMPILER_FLAGS $WARNINGS -c $SRC  $INCLUDE_FOLDERS -o ./build/main.o
ar -crs $OUTPUT ./build/main.o ./dependencies/GLEW/lib/glew.o


cp ./src/*.h ./include/

cp ./build/Renderer.a ./lib/Renderer.a


