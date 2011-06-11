mkdir ..\Build
cd ..\Build
cmake .. -G"MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DFOG_BUILD_EXAMPLES=True -DFOG_BUILD_BENCH=False
cd ..\Scripts
pause
