#!/bin/bash

FOLDER="DvD-0.2"

# Change directory to script dir
cd "${0%/*}"

echo "Creating folder..."
mkdir -p "$FOLDER"

echo "Linking data..."
rm -f "$FOLDER/data"
ln -s ../../../DvD/data "$FOLDER/data"

echo "Bundling binaries..."
cp -f ../../DvD/DvD-32 "$FOLDER/DvD-32"
cp -f ../../DvD/DvD-64 "$FOLDER/DvD-64"

echo "Creating archive..."
rm -f "../$FOLDER.tar.bz2"
tar cjfh "../$FOLDER.tar.bz2" "$FOLDER"
