#!/bin/bash
RESOURCE_PATH="${0%/*}/../Resources"
export DYLD_FALLBACK_LIBRARY_PATH="$RESOURCE_PATH/lib"

cd "$RESOURCE_PATH"
../MacOS/DvD
