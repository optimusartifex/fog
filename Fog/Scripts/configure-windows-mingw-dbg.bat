mkdir ..\Build
cd ..\Build
cmake .. -G"MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DFOG_BUILD_UI=1 -DFOG_BUILD_BENCH=1 -DFOG_BUILD_EXAMPLES=1
cd ..\Scripts
pause
