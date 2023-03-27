# TT2023 - Dau Truong AI

Install dependencies:

```sh
pip install -r requirements.txt
```

Build:

```sh
cmake -S . -B build
cmake --build build -j$(sysctl -n hw.logicalcpu)
```
