# Computer Networking
## Client-Server Chat Application
___
This is a *nix implementation of Computer Networking Project - Phase I - Client Multiplexing

## Dependencies

This project requires the following dependencies for successful build:

1. CMake (`brew install cmake` or `sudo apt install cmake` or equivalent)
2. Xcode command line tools (Mac) or `build-essentials` package (Ubuntu) or equivalent 
3. `ninja` build tool

## Build

From the same directory as this README perform the following steps:
1. `mkdir build && cd build`
2. `cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=ninja -G Ninja -S $PWD/..`