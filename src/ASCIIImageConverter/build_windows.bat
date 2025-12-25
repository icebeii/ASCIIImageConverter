@echo off

cd src
if not exist build mkdir build
cd build
cmake ..
cmake --build .
echo Build finished