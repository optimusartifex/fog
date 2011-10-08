mkdir ..\Build
cd ..\Build
cmake .. -G"MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DFOG_BUILD_BENCH=True -DFOG_BUILD_EXAMPLES=True
cd ..\Scripts
pause
