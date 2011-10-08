#!/bin/sh
mkdir ../Build
cd ../Build
cmake .. -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DFOG_BUILD_UI_X11=1 -DFOG_BUILD_BENCH=True -DFOG_BUILD_EXAMPLES=True
cd ../Scripts
