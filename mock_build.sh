#!/bin/bash
# Build the mock/desktop version of gridlock-808 for headless testing
set -e

SRCDIR=src
MOCKDIR=mock_gba

echo "Compiling mock GBA environment..."
g++ -std=c++17 -O0 -g -Wall -I$MOCKDIR/include -I$SRCDIR -DMOCK_GBA \
    $MOCKDIR/mock_gba.cpp \
    $SRCDIR/syscalls.c \
    $SRCDIR/text.cpp \
    $SRCDIR/audio.cpp \
    $SRCDIR/link.cpp \
    $SRCDIR/grid.cpp \
    $SRCDIR/player.cpp \
    $SRCDIR/main.cpp \
    -o gridlock-mock -lm

echo "Done. Running mock..."
./gridlock-mock 2>&1 | head -50