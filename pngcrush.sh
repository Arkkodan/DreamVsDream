#!/bin/sh
find DvD/res/effects -iname '*.png' -exec convert -strip {} tmp.png \; -exec pngcrush -brute tmp.png tmp2.png \; -exec mv -f tmp2.png {} \; -exec rm -f tmp.png \;
