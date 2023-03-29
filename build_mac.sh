#!/bin/sh

cmake -S . -B build
cmake --build build -j$(sysctl -n hw.logicalcpu)
