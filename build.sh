#!/bin/bash

BUILD=build
DATA=../data

mkdir -p $BUILD
cd $BUILD
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .

./mapbuilder --with-histo -g "de_DE.UTF-8" -p "|" -i ../data/de-vornamen+histo.txt --fill-missing-monograms