#!/usr/bin/env sh

CC=cc
FLAGS="-Wall -Wextra -pedantic-errors -Wfatal-errors"
LIBS="-lssl -lcrypto"
BUILD_DIR=./build
INSTALL_DIR=/usr/local/bin
BIN=castor

case "$1" in
	"--debug")
		FLAGS="${FLAGS} -g"
		;;
	"--clean")
		rm -rv $BUILD_DIR
		exit $?
		;;
	"--uninstall")
		rm -v $INSTALL_DIR/$BIN
		exit $?
		;;
	"--install")
		install -v -m755 $BUILD_DIR/$BIN $INSTALL_DIR/$BIN
		exit $?
		;;
esac

[ ! -d $BUILD_DIR ] && mkdir $BUILD_DIR

for source_file in *.c; do
	$CC $FLAGS -c -o $BUILD_DIR/${source_file%.*}.o $source_file
done

$CC $FLAGS -o $BUILD_DIR/$BIN $BUILD_DIR/*.o $LIBS
