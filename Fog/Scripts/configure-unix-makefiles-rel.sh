#!/bin/sh
mkdir ../Build
cd ../Build
cmake .. -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DFOG_BUILD_UI=1 -DFOG_BUILD_UI_X11=1 -DFOG_BUILD_BENCH=1 -DFOG_BUILD_EXAMPLES=1
cd ../Scripts
