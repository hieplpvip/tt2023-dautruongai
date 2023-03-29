#!/bin/sh

cmake -S . -B build -D CMAKE_BUILD_TYPE=Release
cmake --build build --verbose -j$(sysctl -n hw.logicalcpu)
