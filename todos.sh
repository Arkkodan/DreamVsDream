#!/bin/bash
find . -type f \( -name "*.ubu" -o -name "*.txt" -o -name "*.c" -o -name "*.cpp" -o -name "*.h" \) -exec unix2dos {} +
