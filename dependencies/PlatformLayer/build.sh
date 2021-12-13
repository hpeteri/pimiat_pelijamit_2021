#!/bin/bash


PROJECT="PlatformLayer"
WARNINGS="-Wformat=2 -Wmain -Wmissing-braces -Wparentheses 
          -Wuninitialized -Wfloat-equal -Wsign-compare -Werror"  
LIBS="-lX11 -pthread  -lGL"
COMPILER_FLAGS="-O0"
PREPROCESSOR="-D GLEW_STATIC=1 -D GLEW_NO_GLU=1"
INCLUDE_FOLDERS="-I ./dependencies/GLEW/include/GL/"
# Push directory to <this> location
pushd "`dirname ${BASH_SOURCE[0]}`"


# Create a build, include, and lib folder
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

#Copy all .h files to include

for i in `find src/*.h*`; do
    cp $i include//`basename $i` 
done

# Compile object files without linking
echo "Building $PROJECT ..."

g++ -g $PREPROCESSOR $COMPILER_FLAGS $WARNINGS -c "./src/Main.cpp" $INCLUDE_FOLDERS -o "./build/Main.o" $LIBS

# Create lib
ar -rcs ./build/PlatformLayer_linux.a ./build/Main.o

cp "./build/PlatformLayer_linux.a" "./lib/"

popd
exit 0
