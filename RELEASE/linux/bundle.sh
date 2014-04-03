#!/bin/bash

FOLDER="DvD-0.2"

# Change directory to script dir
cd "${0%/*}"

rm -f "$FOLDER.tar.bz2"
tar cjfh "$FOLDER.tar.bz2" "$FOLDER"
