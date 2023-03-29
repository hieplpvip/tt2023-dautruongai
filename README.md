# TT2023 - Dau Truong AI

## Build

Install dependencies:

```sh
pip install -r requirements.txt
```

Build on Linux:

```sh
./build_linux.sh
```

Build on macOS:

```sh
./build_macos.sh
```

### Cross-compile for Windows on Linux

Install toolchain:

```sh
sudo apt install mingw-w64
```

Build:

```sh
./cross_build_win.sh
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
