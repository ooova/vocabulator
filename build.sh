#!/bin/bash

set -ex

if [[ ! -d "./build" ]]; then
    mkdir ./build
fi

font_path=./assets/fonts

if [[ ! -e ${font_path}/Ubuntu-R.ttf ]]; then
    cp /usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf ${font_path}/
fi

g++ src/main.cc \
    src/tools/string_utils.cc \
    src/ui/main-window.cc \
    src/ui/widgets/card.cc \
    src/vocabulary/vocabulary.cc \
    src/vocabulary/word.cc \
    src/vocabulary/translation.cc \
    -o ./build/main \
    -std=c++20 \
    -Wextra \
    -Wall \
    -Werror \
    -I./src \
    -I./raylib-cpp/include \
    -I./spdlog/include \
    -I/usr/local/include \
    -lraylib \
    -lGL \
    -lm \
    -lpthread \
    -ldl \
    -lrt \
    -lX11 \
    -lfmt \


# -Wshadow

# `pkg-config gtkmm-3.0 --cflags --libs`
