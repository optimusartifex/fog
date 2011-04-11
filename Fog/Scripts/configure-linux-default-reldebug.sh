#!/bin/sh
mkdir ../Build
cd ../Build
cmake .. -DFOG_BUILD_GUI_X11=1 -DCMAKE_BUILD_TYPE=RelWithDebInfo -DFOG_BUILD_EXAMPLES=True -G"Unix Makefiles"
cd ../Scripts
