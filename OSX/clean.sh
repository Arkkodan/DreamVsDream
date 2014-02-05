#!/bin/bash

NAME="DvD"

PATH_APP="$NAME.app/Contents/MacOS"
PATH_RES="$PATH_APP/res"
PATH_LIB="$PATH_APP/libs"
PATH_BIN="$PATH_RES/DvD"

# Change directory to script dir
cd "${0%/*}"

rm -rf DvD DvD_x86 DvD_x64 DvD.exe "$PATH_RES"
