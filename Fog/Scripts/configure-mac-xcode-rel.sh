#!/bin/sh
mkdir ../Build
cd ../Build
cmake .. -G"Xcode" -DFOG_BUILD_GUI_X11=1 -DCMAKE_BUILD_TYPE=Release -DFOG_BUILD_EXAMPLES=True
cd ../Scripts
