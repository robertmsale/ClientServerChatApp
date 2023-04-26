#!/usr/bin/env bash

source ./env

# Here you can change the port if you want to
PORT=33420

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
## Get CPU Cores

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

## Setup defaults

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

# Provide default client number
if [[ -z "$NUM_OF_CLIENTS" ]]
then
  NUM_OF_CLIENTS=2
fi

## Setup build folder

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

IP=127.0.0.1

function SendLetters() {
  grep -o . <<< "$1" | while read letter; do tmux send-keys -t "$2" "$letter"; done
  tmux send-keys -t "$2" Enter
}

if which tmux | grep -q -o "not found"
then
  echo "tmux not installed, but build files can be accessed from $BUILD_FOLDER!"
else

  tmux new-session -s "$SESSION_NAME" -n "$SESSION_NAME" -d
  tmux split-window -h
  for i in $(seq 1 $NUM_OF_CLIENTS)
  do
    if [[ $i -ne $NUM_OF_CLIENTS ]]
    then
      tmux split-window -v
    fi
  done

  # execute apps in each pane
  tmux send-keys -t 0 "./server $PORT" C-m
  for i in $(seq 1 $NUM_OF_CLIENTS)
  do
  tmux send-keys -t $i "./client" C-m
  done

  sleep 0.5

  # automatically enter connection info (works by looping each char and sending one at a time)
  for i in $(seq 1 $NUM_OF_CLIENTS)
  do
    SendLetters $IP $i
    sleep 0.1
    SendLetters $PORT $i
    sleep 0.1
  done

  # turn on mouse only for this session's window
  tmux setw -g mouse on

  tmux attach-session -t "$SESSION_NAME"

  tmux kill-session -t "$SESSION_NAME"

fi
popd