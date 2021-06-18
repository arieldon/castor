#!/usr/bin/env sh

CC=cc
FLAGS="-g -Wall -Wextra -pedantic-errors -Wfatal-errors"
LIBS="-lssl -lcrypto"
BUILD_DIR=./build
BIN=castor

[ \( "$1" = "--clean" \) -a \( -d $BUILD_DIR \) ] && rm -r ./build && exit 0
[ ! -d $BUILD_DIR ] && mkdir ./build

for source_file in *.c; do
	$CC $FLAGS -c -o ./build/${source_file%.*}.o $source_file 
done

$CC $FLAGS -o $BUILD_DIR/$BIN $BUILD_DIR/*.o $LIBS
