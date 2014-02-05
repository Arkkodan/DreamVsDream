#!/bin/bash

for dir in *; do
	if [ -d "$dir" ]; then
		pushd "$dir" > /dev/null
		for file in *.png; do
			echo "$file" | sed 's/^[^0-9]*0*//' | xargs -L 1 -I {} mv -f "$file" {} 2>/dev/null
		done
		popd > /dev/null
	fi
done
