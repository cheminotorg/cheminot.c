# cheminot.c

`cheminot.c` est une librarie C++ qui implemente une réponse au problème TDSP (Time-Dependent Shortest Paths over Large Graphs).
Pour plus d'information sur ce problème, je vous invite à lire le papier présent dans le répertoire `paper`.

Cette librairie est utilisée par l'application mobile [cheminot.m](https://github.com/cheminotorg/cheminot.m) et plus précisément le plugin cordova [m.cheminot.plugin](https://github.com/cheminotorg/m.cheminot.plugin).

## Requirements

`cheminot.c` possède les dépendances suivantes:
- [jsoncpp](https://github.com/open-source-parsers/jsoncpp)
- [protobuf](https://github.com/google/protobuf)
- [gtests](https://code.google.com/p/googletest/)

`protobuf` est la seule dépendances qui n'est pas inclut directement dans ce dépôt. Vous allez devoir la construire vous même.
Si vous êtes sur Mac OS, `protobuf` requiert que vous installiez `autoconf`:

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

Pour inclure `protobuf` dans ce projet il suffit:

```shell
cd lib
ln -s <path to protobuf directory> protobuf
```

## Construire `cheminot.c`

Lancez `cmake` puis `make`:

```shell
cmake .
make
```
