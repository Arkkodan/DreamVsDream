#!/bin/bash

NAME="Dream vs. Dream"

PATH_APP="$NAME.app/Contents"

PATH_DATA="$PATH_APP/Resources/data"
PATH_LIB="$PATH_APP/Resources/lib"

# Change directory to script dir
cd "${0%/*}"

echo "Bundling resources..."
rm -rf "$PATH_DATA"
cp -rf "../DvD/data" "$PATH_RES"

echo "Bundling libraries..."
rm -rf "$PATH_LIB" && mkdir -p "$PATH_LIB"
#cp -fL "/opt/local/lib/"

echo "Bundling libc++..."
cp -fL "/opt/local/lib/libc++.1.dylib" "$PATH_LIB"
cp -fL "/opt/local/lib/libc++abi.dylib" "$PATH_LIB"

echo "Done!"
