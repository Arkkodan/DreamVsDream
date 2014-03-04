#!/bin/bash

#Create atlas
echo "Creating atlas..."
rm -rf "chars/$1/atlas"
mkdir -p "chars/$1/atlas"
./atlas -o "chars/$1/atlas" chars/"$1"/sprites/*.png

#Compile character
echo "Compiling character..."
./compiler "$1"

#Move
echo "Moving character..."
mv -f "chars/$1.char" ../DvD/data/chars

echo "Done!"
