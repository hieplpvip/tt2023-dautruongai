#!/bin/sh

cmake -DCMAKE_TOOLCHAIN_FILE=mingw-w64-toolchain.cmake -S . -B build
cmake --build build -j$(nproc)
