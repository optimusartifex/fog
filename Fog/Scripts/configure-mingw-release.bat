mkdir ..\Build
cd ..\Build
cmake .. -DCMAKE_BUILD_TYPE=Release -DFOG_BUILD_EXAMPLES=True -DFOG_BUILD_BENCH=False -G"MinGW Makefiles"
cd ..\Scripts
pause
