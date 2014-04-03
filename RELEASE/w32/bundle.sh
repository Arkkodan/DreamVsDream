#!/bin/bash

NAME="DvD-0.2"

# Change directory to script dir
cd "${0%/*}"

echo "Making directories..."
mkdir -p x86 x64

echo "Linking data..."
rm -f x86/data x64/data
ln -s ../../../DvD/data x86/data
ln -s ../../../DvD/data x64/data

echo "Bundling binaries..."
cp -f ../../DvD/DvD-32.exe x86/DvD.exe
cp -f ../../DvD/DvD-64.exe x64/DvD.exe

echo "Creating archives..."
rm -f "../$NAME-x86.zip" "../$NAME-x64.zip"
pushd x64 >/dev/null && zip -r "../../$NAME-x86.zip" . >/dev/null && popd >/dev/null
pushd x86 >/dev/null && zip -r "../../$NAME-x64.zip" . >/dev/null && popd >/dev/null
