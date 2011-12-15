#!/bin/sh
mkdir ../Build
cd ../Build
cmake .. -DCMAKE_TOOLCHAIN_FILE="../Scripts/iOS.cmake" -DIOS_PLATFORM="SIMULATOR" -G"Xcode"
cd ../Scripts
