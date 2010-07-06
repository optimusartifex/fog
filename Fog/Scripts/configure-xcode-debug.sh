#!/bin/sh
mkdir ../Build
cd ../Build
cmake .. -DFOG_BUILD_GUI_X11=1 -DCMAKE_BUILD_TYPE=Debug -DFOG_DEBUG=1 -DFOG_DEBUG_MEMORY=1 -DFOG_BUILD_EXAMPLES=True -G"Xcode"
cd ../Scripts
