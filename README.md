# TT2023 - Dau Truong AI

Install dependencies:

```sh
pip install -r requirements.txt
```

Build:

```sh
cmake -S . -B build
cmake --build build
```

## Cross-compile for Windows

Install toolchain:

```sh
sudo apt install mingw-w64
```

Build:

```sh
cmake -DCMAKE_TOOLCHAIN_FILE=mingw-w64-toolchain.cmake -S . -B build
cmake --build build
```
