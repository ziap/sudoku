#!/bin/sh -xe

CC="${CC:-clang}"
CFLAGS="-std=c99 -Wall -Wextra -pedantic"
LDLIBS=""

BUILD_FLAGS="-O3 -march=native -mtune=native -s"
DEBUG_FLAGS="-Og -ggdb"

SRCS="$(ls src/*.c)"

$CC $CFLAG $LDLIBS $BUILD_FLAGS -o sudoku main.c $SRCS -I src
