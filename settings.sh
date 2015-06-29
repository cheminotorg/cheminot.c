#!/bin/bash$

VERSION=$(git describe --long --always)

echo "namespace cheminotc { static std::string gitVersion = \"$VERSION\"; }" > $1/settings.h
