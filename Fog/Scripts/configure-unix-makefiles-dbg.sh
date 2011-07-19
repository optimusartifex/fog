#!/bin/sh
mkdir ../Build
cd ../Build
cmake .. -G"Unix Makefiles" -DFOG_BUILD_GUI_X11=1 -DCMAKE_BUILD_TYPE=Debug -DFOG_BUILD_EXAMPLES=True
cd ../Scripts
