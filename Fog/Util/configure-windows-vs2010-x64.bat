mkdir ..\Build
cd ..\Build
cmake .. -G"Visual Studio 10 Win64" -DFOG_BUILD_UI=1 -DFOG_BUILD_BENCH=1 -DFOG_BUILD_EXAMPLES=1
cd ..\Util
pause
