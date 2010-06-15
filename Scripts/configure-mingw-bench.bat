mkdir ..\Build
cd ..\Build
cmake .. -DCMAKE_BUILD_TYPE=Release -DFOG_BUILD_BENCH=True -DFOG_BUILD_EXAMPLES=True -G"MinGW Makefiles"
cd ..\Scripts
pause
