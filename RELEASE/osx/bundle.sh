#!/bin/bash

NAME="Dream vs. Dream"

PATH_APP="$NAME.app/Contents"

PATH_BIN="$PATH_APP/MacOS/DvD"
PATH_DATA="$PATH_APP/Resources/data"
PATH_LIB="$PATH_APP/Resources/lib"

PATH_LIB_SYS="/opt/local/lib"

# Change directory to script dir
cd "${0%/*}"

echo "Bundling binary..."
rm -f "$PATH_BIN" && cp -f "../../DvD/DvD" "$PATH_BIN"

echo "Bundling resources..."
rm -rf "$PATH_DATA"
cp -rf "../DvD/data" "$PATH_DATA"

echo "Bundling libraries (level 1)..."
rm -rf "$PATH_LIB" && mkdir -p "$PATH_LIB"
cp -fL "$PATH_LIB_SYS/libSDL2-2.0.0.dylib" "$PATH_LIB"
cp -fL "$PATH_LIB_SYS/libpng16.16.dylib" "$PATH_LIB"
cp -fL "$PATH_LIB_SYS/libgif.6.dylib" "$PATH_LIB"
cp -fL "$PATH_LIB_SYS/libsndfile.1.dylib" "$PATH_LIB"
cp -fL "$PATH_LIB_SYS/libc++.1.dylib" "$PATH_LIB"
cp -fL "$PATH_LIB_SYS/libc++abi.dylib" "$PATH_LIB"

echo "Bundling libraries (level 2)..."
cp -fL "$PATH_LIB_SYS/libFLAC.8.dylib" "$PATH_LIB"
cp -fL "$PATH_LIB_SYS/libvorbisenc.2.dylib" "$PATH_LIB"
cp -fL "$PATH_LIB_SYS/libvorbis.0.dylib" "$PATH_LIB"
cp -fL "$PATH_LIB_SYS/libogg.0.dylib" "$PATH_LIB"

echo "Done!"
