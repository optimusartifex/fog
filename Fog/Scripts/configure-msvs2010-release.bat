mkdir ..\Build
cd ..\Build
cmake .. -DCMAKE_BUILD_TYPE=Release -DFOG_BUILD_EXAMPLES=True -G"Visual Studio 10"
cd ..\Scripts
pause
