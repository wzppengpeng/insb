#!/bin/bash


# clean the environment
rm -rf build
rm -rf bin
rm -rf lib

# build the project
mkdir -p build

cd build

cmake ..

make -j8

cd ..