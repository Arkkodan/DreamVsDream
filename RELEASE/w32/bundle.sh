#!/bin/bash

NAME="DvD-0.2"

# Change directory to script dir
cd "${0%/*}"

rm -f "$NAME-x86.zip" "$NAME-x64.zip"
pushd x64 >/dev/null && zip -r "../$NAME-x86.zip" . >/dev/null && popd >/dev/null
pushd x86 >/dev/null && zip -r "../$NAME-x64.zip" . >/dev/null && popd >/dev/null
