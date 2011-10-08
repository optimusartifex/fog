mkdir ..\Build
cd ..\Build
cmake .. -G"Visual Studio 9 2008" -DFOG_BUILD_BENCH=True -DFOG_BUILD_EXAMPLES=True
cd ..\Scripts
pause
