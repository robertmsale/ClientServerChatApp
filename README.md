# Computer Networking
## Client-Server Chat Application
___
This is a *nix implementation of Computer Networking Chat App Project

## Dependencies

This project requires the following dependencies for successful build:

1. Homebrew (`/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"`) or equivalent package manager for *nix system
2. CMake (`brew install cmake` or `sudo apt install cmake` or equivalent)
3. Xcode command line tools (Mac) or `build-essentials` package (Ubuntu) or equivalent 
4. `ninja` build tool

Optionally, but highly recommended, `tmux`.

## Build

### Convenient method

From the same directory as this README, execute:
```bash
./build-and-start.sh # [build folder] [session name] <- optionals
```

This will build the project using CMake and fire up a tmux session with 1 server and 2 client panes automatically. It also enables mouse control for tmux to make it *even easier* 😁 also by running this script, if you detach `CTRL+b d` from the session it will automatically kill the session for you.

If `tmux` is not installed it will skip that part entirely.

### From Scratch

From the same directory as this README perform the following steps:
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=ninja -G Ninja -S $PWD/..
cmake --build ./ --target all -j 4 # or however many number of CPU cores you want to use
```

## IMPORTANT NOTE

Because this program disables echo and canonical input on stdin, using `CTRL+C` does not work, so running these in their own tmux session is highly recommended so you can `CTRL+b x y` to terminate the program in case a deadlock occurs.