mkdir ..\Build
cd ..\Build
cmake .. -G"Visual Studio 10 Win64" -DFOG_BUILD_BENCH=True -DFOG_BUILD_EXAMPLES=True
cd ..\Scripts
pause
