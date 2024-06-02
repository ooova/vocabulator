#!/bin/bash

set -ex

if [[ ! -d "./build" ]]; then
    mkdir ./build
fi

g++  src/main.cc \
    -o ./build/main \
    -I./spdlog/include \
    -I./src \
    -I/usr/local/include \
    -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 \
    -std=c++11 \
    -Wextra \
    -Wall \
    -Wshadow

# `pkg-config gtkmm-3.0 --cflags --libs`