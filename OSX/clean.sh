#!/bin/bash

NAME="Dream vs. Dream"

PATH_APP="$NAME.app/Contents"

PATH_BIN="$PATH_APP/MacOS/DvD"
PATH_DATA="$PATH_APP/Resources/data"
PATH_LIB="$PATH_APP/Resources/lib"

# Change directory to script dir
cd "${0%/*}"

rm -rf "$PATH_BIN" "$PATH_DATA" "$PATH_LIB"
