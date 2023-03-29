#!/bin/sh

cmake -S . -B build -D CMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=mingw-w64-toolchain.cmake
cmake --build build --verbose -j$(nproc)
