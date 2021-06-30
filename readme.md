# Castor
Castor is a simple [Gemini](https://gemini.circumlunar.space/) client.

## Usage
Usage is simple -- the program takes a URL that complies with the Gemini
protocol (gemini://...) as its sole argument.
```
$ castor gemini://gemini.circumlunar.space
```

## Compile
Without any additional arguments, `build.sh` creates directory `./build` and 
fills it with the results of compilation, including the executable file.
```
$ ./build.sh
$ ./build/castor gemini://gemini.circumlunar.space
```

## Install
By default, the program installs to `/usr/local/bin`. However, the directory 
can be modified by changing variable `INSTALL_DIR` in `build.sh`.
```
$ ./build.sh
$ ./build.sh --install
$ castor gemini://gemini.circumlunar.space
```

## Uninstall
```
$ ./build.sh --uninstall
```
