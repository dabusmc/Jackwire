@echo off

cd ..
if not exist build/ mkdir build

cmake -S . -B build -G "MinGW Makefiles"
cd build
mingw32-make

pause