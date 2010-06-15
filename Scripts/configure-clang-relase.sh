#!/bin/sh
mkdir ../Build
cd ../Build
cmake .. -DCMAKE_CXX_COMPILER=/usr/local/bin/clang -DFOG_BUILD_GUI_X11=1 -DCMAKE_BUILD_TYPE=Release -DFOG_BUILD_EXAMPLES=True -G"Unix Makefiles"
cd ../Scripts
