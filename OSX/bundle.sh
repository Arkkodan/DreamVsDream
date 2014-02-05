#!/bin/bash

NAME="DvD"

PATH_APP="$NAME.app/Contents/MacOS"
PATH_RES="$PATH_APP/res"
PATH_LIB="$PATH_APP/libs"
PATH_BIN="$PATH_RES/DvD"

# Change directory to script dir
cd "${0%/*}"

echo "Bundling resources..."
rm -rf "$PATH_RES"
cp -rf ../DvD/res "$PATH_RES"
lipo -create DvD_x86 DvD_x64 -output "$PATH_BIN"

echo "Done!"
