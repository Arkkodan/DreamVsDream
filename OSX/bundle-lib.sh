#!/bin/bash

NAME="DvD"

PATH_APP="$NAME.app/Contents/MacOS"
PATH_RES="$PATH_APP/res"
PATH_LIB="$PATH_APP/libs"
PATH_BIN="$PATH_RES/DvD"

echo "Bundling libraries..."
rm -rf "$PATH_LIB"
mkdir -p "$PATH_LIB"
dylibbundler -od -b -x "$PATH_BIN" -d "$PATH_LIB"

echo "Bundling libc++..."
cp /opt/local/lib/libc++.1.dylib "$PATH_LIB"
cp /opt/local/lib/libc++abi.dylib "$PATH_LIB"

# update libs
install_name_tool -id @executable_path/../libs/libc++.1.dylib "$PATH_LIB/libc++.1.dylib"
install_name_tool -change /usr/lib/libc++abi.dylib @executable_path/../libs/libc++abi.dylib "$PATH_LIB/libc++.1.dylib"
install_name_tool -id @executable_path/../libs/libc++abi.dylib "$PATH_LIB/libc++abi.dylib"

# update DvD binary
install_name_tool -change /usr/lib/libc++.1.dylib @executable_path/../libs/libc++.1.dylib "$PATH_BIN"

echo "Done!"
