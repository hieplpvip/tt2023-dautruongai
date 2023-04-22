#!/bin/sh

cmake -S . -B build -D CMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=mingw-w64-toolchain.cmake -DPRINT_FROM_SENO_WITH_LOVE=ON -DTUNE_FOR_X86=ON
cmake --build build --verbose -j$(nproc)
