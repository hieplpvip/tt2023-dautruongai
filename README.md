# TT2023 - Dau Truong AI

## Build

Must run on Linux or macOS.

Install dependencies:

```sh
pip install -r requirements.txt
```

Build:

```sh
cmake -S . -B build
cmake --build build
```

### Cross-compile for Windows

Install toolchain:

```sh
sudo apt install mingw-w64
```

Build:

```sh
cmake -DCMAKE_TOOLCHAIN_FILE=mingw-w64-toolchain.cmake -S . -B build
cmake --build build
```

## Run

Run once and visualize:

```sh
judge/run.py maps/sample/map_01.txt bin/heisenberg bin/pinkman --visualize visualization
```

Run multiple times:

```sh
judge/run_many.py maps/list_semifinal.txt bin/heisenberg bin/pinkman --times 5
```
