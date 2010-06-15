mkdir ..\Build
cd ..\Build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DFOG_BUILD_EXAMPLES=True -G"Visual Studio 6"
cd ..\Scripts
pause
