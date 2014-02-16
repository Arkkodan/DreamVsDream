#!/bin/bash

for dir in *; do
	if [ -d "$dir" ]; then
		pushd "$dir" > /dev/null
		for file in *.png; do
			file_new=`echo $file | grep -o '[1-9][0-9]*.png$'`
			if [ "$file_new" != "$file" ]; then
				echo "$file > $file_new"
				convert -strip -background '#000000' -alpha remove "$file" tmp.png 2>/dev/null
				pngcrush -brute tmp.png "$file_new" 2>/dev/null
				rm -f "$file" tmp.png
			fi
		done
		popd > /dev/null
	fi
done
