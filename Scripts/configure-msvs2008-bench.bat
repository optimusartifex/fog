mkdir ..\Build
cd ..\Build
cmake .. -DCMAKE_BUILD_TYPE=Release -DFOG_BUILD_BENCH=True -DFOG_BUILD_EXAMPLES=True -G"Visual Studio 9 2008"
cd ..\Scripts
pause
