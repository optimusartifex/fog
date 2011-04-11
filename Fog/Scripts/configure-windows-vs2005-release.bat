mkdir ..\Build
cd ..\Build
cmake .. -DCMAKE_BUILD_TYPE=Release -DFOG_BUILD_EXAMPLES=True -G"Visual Studio 8 2005"
cd ..\Scripts
pause
