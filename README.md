# Surakarta Core

This repository is a library for https://github.com/surakarta-game/surakarta-game.

Here you can find the source code for libsurakarta, a library that provides rule management and a simple AI for https://github.com/surakarta-game/surakarta-game, together with a simple CLI tool for benchmark and some unit tests for the library.

Use the following command to clone the repository with dependencies:
```bash
git clone https://github.com/surakarta-game/surakarta-core.git --recursive
```

## How to compile

You can use GCC, Clang or MSVC to build the project on Linux (or other UNIX-like OS) or Windows. Unit tests are not available for Windows.

### Linux

You can compile the project on UNIX-like OS with the following commands:
```bash
mkdir build
cd build
cmake ..
make
```

Binary outputs can be found in build/bin and build/lib:
 - build/bin/surakarta-benchmark: a simple CLI tool for benchmark
 - build/bin/surakarta-test: unit tests
 - build/lib/libsurakarta.a: the library that contains rule manager and the simple AI

 ### Windows

 You can use Visual Studio to compile this repository. VS supports CMake very well, and we have tested to build this CMake project with MSVC.

 ## Dependencies

 All the dependencies are used for testing. So if you don't want to test, all the dependencies are unnecessary.
