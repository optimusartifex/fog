#!/bin/sh
mkdir ../Build
cd ../Build
cmake .. -G"Xcode" -DCMAKE_BUILD_TYPE=Release -DFOG_BUILD_EXAMPLES=True
cd ../Scripts
