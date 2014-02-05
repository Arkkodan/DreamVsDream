#!/bin/bash
find . -type f \( -name "*.ubu" -o -name "*.txt" -o -name "*.c" -o -name "*.cpp" -o -name "*.h" -o -name "*.glsl" \) -exec dos2unix {} +
