#!/bin/bash

NAME="DvD"

PATH_APP="$NAME.app/Contents/MacOS"
PATH_RES="$PATH_APP/res"

# Change directory to script dir
cd "${0%/*}"

rm -rf "$PATH_RES"
