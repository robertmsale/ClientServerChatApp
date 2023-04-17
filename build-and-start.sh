#!/usr/bin/env bash

# $1: build folder
# $2: tmux session name
BUILD_FOLDER="$1"
SESSION_NAME="$2"

NUM_OF_CPU_CORES=1

#+------------------------------------------------------------------+
#|              #####  ####### ####### #     # ######               |
#|             #     # #          #    #     # #     #              |
#|             #       #          #    #     # #     #              |
#|              #####  #####      #    #     # ######               |
#|                   # #          #    #     # #                    |
#|             #     # #          #    #     # #                    |
#|              #####  #######    #     #####  #                    |
#+------------------------------------------------------------------+
# To speed up compilation, retrieve the number of CPU cores to use by compiler
if which sysctl | grep -q -o "not found" # if using MacOS
then
  NUM_OF_CPU_CORES="$(sysctl -n hw.ncpu)"
elif which nproc | grep -q -o "not found" # if using Linux with coreutils installed
then
  NUM_OF_CPU_CORES="$(nproc --all)"
else # if using Linux of any kind
  NUM_OF_CPU_CORES="$(grep -c '^processor' /proc/cpuinfo)"
fi

# don't use all cores
NUM_OF_CPU_CORES="$((NUM_OF_CPU_CORES - 2))"

# use at least one core
if [[ NUM_OF_CPU_CORES -lt 1 ]]
then
  NUM_OF_CPU_CORES=1
fi

# Provide default session name
if [[ -z "$SESSION_NAME" ]]
then
  SESSION_NAME="ClientServerChatApp"
fi

# Provide default build folder
if [[ -z "$BUILD_FOLDER" ]]
then
  BUILD_FOLDER="./build"
fi

mkdir -p $BUILD_FOLDER
# shellcheck disable=SC2164
pushd $BUILD_FOLDER

#+------------------------------------------------------------------+
#|               ######  #     # ### #       ######                 |
#|               #     # #     #  #  #       #     #                |
#|               #     # #     #  #  #       #     #                |
#|               ######  #     #  #  #       #     #                |
#|               #     # #     #  #  #       #     #                |
#|               #     # #     #  #  #       #     #                |
#|               ######   #####  ### ####### ######                 |
#+------------------------------------------------------------------+

cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=ninja -G Ninja -S "$PWD/.."
cmake --build ./ --target all -j "$NUM_OF_CPU_CORES"

#+------------------------------------------------------------------+
#|                     ######  #     # #     #                      |
#|                     #     # #     # ##    #                      |
#|                     #     # #     # # #   #                      |
#|                     ######  #     # #  #  #                      |
#|                     #   #   #     # #   # #                      |
#|                     #    #  #     # #    ##                      |
#|                     #     #  #####  #     #                      |
#+------------------------------------------------------------------+

if which tmux | grep -q -o "not found"
then
  echo "tmux not installed, but build files can be accessed from $BUILD_FOLDER!"
else

  tmux new-session -s "$SESSION_NAME" -n "$SESSION_NAME" -d
  tmux split-window -h
  tmux split-window -v

  # execute apps in each pane
  tmux send-keys -t 0 "./server 33420" C-m
  tmux send-keys -t 1 "./client" C-m
  tmux send-keys -t 2 "./client" C-m

  # turn on mouse
  tmux setw -g mouse on

  tmux attach-session -t "$SESSION_NAME"

  tmux kill-session -t "$SESSION_NAME"

fi
popd