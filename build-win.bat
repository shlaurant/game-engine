@echo off

if not exist build mkdir build

rem generate cmake files
cmake -O . -B ./build -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=ON

rem copy assets and .dll files to output mkdir
robocopy "./resource" "./build/bin/Debug/assets" /E
robocopy "./windows/bin/" "./build/bin/Debug" /E

cd build && cmake --build
echo compilation finished!