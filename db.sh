#!/bin/bash

ROOT=$(pwd)
VERSION=$1

if [ -z "$VERSION" ]
then
    echo "Please provide version"
else
    rm cheminot.db
    rm inter-calendardates
    rm inter-graph
    rm ter-calendardates
    rm ter-graph
    rm trans-calendardates
    rm trans-graph

    ln -s ../cheminot.db/db/current/cheminot-$VERSION.db cheminot.db
    ln -s ../cheminot.db/db/current/inter-calendardates-$VERSION inter-calendardates
    ln -s ../cheminot.db/db/current/inter-graph-$VERSION inter-graph
    ln -s ../cheminot.db/db/current/ter-calendardates-$VERSION ter-calendardates
    ln -s ../cheminot.db/db/current/ter-graph-$VERSION ter-graph
    ln -s ../cheminot.db/db/current/trans-calendardates-$VERSION trans-calendardates
    ln -s ../cheminot.db/db/current/trans-graph-$VERSION trans-graph
fi
