#!/bin/bash$

VERSION=$(git describe --long --always)

echo "namespace cheminotc { static std::string gitVersion = \"$VERSION\"; }" > src/settings.h
