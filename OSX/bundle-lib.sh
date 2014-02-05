#!/bin/bash

NAME="DvD"

PATH_APP="$NAME.app/Contents/MacOS"
PATH_RES="$PATH_APP/res"
PATH_LIB="$PATH_APP/libs"
PATH_BIN="$PATH_RES/DvD"

echo "Bundling libraries..."
dylibbundler -od -b -x "$PATH_BIN" -d "$PATH_LIB"

echo "Done!"
