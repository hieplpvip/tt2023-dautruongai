#!/bin/sh

BIN_DIR=`realpath bin`

env PGO_FLAGS="-fprofile-dir=$BIN_DIR -fprofile-use=$BIN_DIR" cmake -S . -B build -D CMAKE_BUILD_TYPE=Release
cmake --build build --verbose -j1
