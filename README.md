# cheminot.c

`cheminot.c` is a C++ library that implement an answer to the TDSP problem (Time-Dependent Shortest Paths over Large Graphs).
You can find a paper related to the algorythm in the `paper` directory of this repository.
This project part of a more general project named `cheminot`.

## Requirements

`cheminot.c` has the following dependencies:
- [jsoncpp](https://github.com/open-source-parsers/jsoncpp)
- [protobuf](https://github.com/google/protobuf)
- [gtests](https://code.google.com/p/googletest/)

`protobuf` is the only one which is not ship with the code.
You will have to build it on your own.
If you are on Mac OS, `protobuf` needs you install `autoconf`:

```shell
curl -OL http://ftpmirror.gnu.org/autoconf/autoconf-2.69.tar.gz
tar -xzf autoconf-2.69.tar.gz
cd autoconf-2.69
./configure && make && sudo make install

curl -OL http://ftpmirror.gnu.org/automake/automake-1.14.tar.gz
tar -xzf automake-1.14.tar.gz
cd automake-1.14
./configure && make && sudo make install

curl -OL http://ftpmirror.gnu.org/libtool/libtool-2.4.2.tar.gz
tar -xzf libtool-2.4.2.tar.gz
cd libtool-2.4.2
./configure && make && sudo make install
```

Once `protobuf` is built, include it in the project:

```shell
cd lib
ln -s <path to protobuf directory> protobuf
```

## Build

Using `make` and `cmake`:

```shell
cmake .
make
```

## Tests

TODO
